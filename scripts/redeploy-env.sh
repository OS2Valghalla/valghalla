#!/usr/bin/env bash

SCRIPTS_DIR=/var/www/.os2subsite_provision
SUFFIX=.adm.valghalla.dk

BASEDIR=$(cd ../ && pwd)
SITENAME=$(echo "$1" | tr -d ' ')

if [ -z "$SITENAME" ]
  then
    echo "ERROR: New site name is empty."
    exit 10
fi

HOST=$SITENAME$SUFFIX

# Breakpoint function to get confirmation to continue.
ask_continue() {
    while true; do
    read -p "Would you like to continue? [y/n] " yn
    case $yn in
        [Yy]* ) echo "Continuing..."; break;;
        [Nn]* ) exit 10;;
        * ) echo "Please answer y or no.";;
    esac
done
}

echo "Check settings before start"
echo ${BASEDIR}/public_html/sites/${HOST}
ask_continue;

echo Fetching db_dump...
mkdir -p $BASEDIR/db_dumps
scp valghalla:/var/lib/mysql_backup/valg_${SITENAME}_bellcom_dk.sql.gz $BASEDIR/db_dumps/
scp valghalla:/var/lib/mysql_backup/valg_${SITENAME}_bellcom_dk-cache-structure.sql.gz $BASEDIR/db_dumps/
ask_continue;

cd ${BASEDIR}/public_html/sites/${HOST}

echo Upload db dump...
drush sql-drop -y
zcat < ${BASEDIR}/db_dumps/valg_${SITENAME}_bellcom_dk.sql.gz | drush sqlc
zcat < ${BASEDIR}/db_dumps/valg_${SITENAME}_bellcom_dk-cache-structure.sql.gz | drush sqlc

echo Setting variables...
drush vset file_private_path sites/${HOST}/private/files
drush vset file_public_path  sites/${HOST}/files
drush vset file_temporary_path ${BASEDIR}/tmp/${HOST}

echo Fixing permissons...
drush cc all && sudo fix-www-permissions.sh && drush composer-json-rebuild &&  sudo fix-www-permissions.sh

echo Fix missing modules...
sh ${BASEDIR}/scripts/fix-missing-modules.sh
ask_continue;

#echo Copying files...
#scp -r valghalla:/var/www/valghalla/public_html/sites/valg-${SITENAME}.bellcom.dk/files/* ./files
#ask_continue;

echo Updating db...
drush updb -y
ask_continue;

echo Dumping db...
drush sql-dump | gzip -9 > ${BASEDIR}/db_dumps/${SITENAME}_$(date +%Y%m%d).sql.gz

echo Final fix permission...
sudo fix-www-permissions.sh
