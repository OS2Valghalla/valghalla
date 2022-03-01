#!/usr/bin/env bash

SCRIPTS_DIR=/var/www/.os2subsite_provision
SUFFIX=.valghalla.dk
EMAIL=domain@bellcom.dk
DB_DUMP_PATH=/var/www/valghalla-internal/db_dumps

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
echo Site_path: ${BASEDIR}/public_html/sites/${HOST}
echo Db dump path: ${DB_DUMP_PATH}
ask_continue;


echo Creating instance instance...
sudo BASEDIR=${BASEDIR} bash ${SCRIPTS_DIR}/subsite_migrate.sh ${HOST} ${EMAIL}
cd ${BASEDIR}/public_html/sites/${HOST}
echo $(pwd)
drush status
ask_continue;

echo Upload db dump...
drush sql-drop -y
zcat < ${DB_DUMP_PATH}/${SITENAME}_$(date +%Y%m%d).sql.gz | drush sqlc

echo Setting variables...
drush vset file_private_path sites/${HOST}/private/files
drush vset file_public_path  sites/${HOST}/files
drush vset file_temporary_path ${BASEDIR}/tmp/${HOST}

echo Fixing permissons...
drush cc all && sudo fix-www-permissions.sh && drush composer-json-rebuild &&  sudo fix-www-permissions.sh
ask_continue;

echo Copying files...
scp -r valghalla:/var/www/valghalla/public_html/sites/valg-${SITENAME}.bellcom.dk/files/* ./files
ask_continue;

echo Disabling modules that not used on external server...
drush dis -y liste_beskeder liste_frivillige_uden_email liste_m_cpr_nummer liste_parti_oversigt liste_valghalla_export liste_valghalla_kvittering locationmap valghalla_eboks valghalla_lists valghalla_mail valghalla_notifications valghalla_signup valghalla_signup_list valghalla_sms valghalla_volunteer_validator valghalla_volunteers_import valghalla_volunteers_invite vcv_serviceplatformen vcv_person_lookup_extended vvv_validate_age field_ui views_ui ckeditor imce mailsystem mimemail panels_ipe panels_mini dashboard search contextual color contact valghalla_status_report

echo Uninstalling modules that not used on external server...
drush pmu -y valghalla_eboks valghalla_mail valghalla_sms stylizer liste_beskeder liste_frivillige_uden_email liste_m_cpr_nummer liste_parti_oversigt liste_valghalla_export liste_valghalla_kvittering
drush pmu -y mimemail
drush pmu -y locationmap valghalla_lists valghalla_notifications valghalla_signup valghalla_signup_list valghalla_volunteer_validator valghalla_volunteers_import valghalla_volunteers_invite vcv_serviceplatformen vcv_person_lookup_extended vvv_validate_age field_ui views_ui ckeditor imce mailsystem mimemail panels_ipe panels_mini dashboard search contextual color contact valghalla_status_report

ask_continue;

echo Enabling external server module...
drush en valghalla_external_server -y

echo Final fix permission...
sudo fix-www-permissions.sh

echo ${HOST} salt
less settings.php | grep "drupal_hash_salt = '"
