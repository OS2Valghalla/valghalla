#!/usr/bin/env bash
FILE=/tmp/health-sync-check.log
SENDTO=ay@bellcom.dk
DRUSH=$(which drush)

echo $(date) > $FILE
echo "\n" >> $FILE

echo "Counter sync matching check ..." >> $FILE
$DRUSH @sites --root=/var/www/valghalla-internal/public_html ev "if (function_exists('valghalla_internal_server_synch_check')) {valghalla_internal_server_synch_check();}" -y >> $FILE 2>&1

echo "\n" >> $FILE
echo "\n" >> $FILE
echo "Overloaded seat matrix check ..." >> $FILE
$DRUSH @sites --root=/var/www/valghalla-internal/public_html ev "if (function_exists('odin_volunteers_full_counts')) {odin_volunteers_full_counts(1);}" -y >> $FILE 2>&1
$DRUSH @sites --root=/var/www/valghalla-external/public_html ev "if (function_exists('odin_volunteers_full_counts')) {odin_volunteers_full_counts(1);}" -y >> $FILE 2>&1

echo "\n" >> $FILE
echo "\n" >> $FILE
echo "Broken field collection check ..." >> $FILE
$DRUSH @sites --root=/var/www/valghalla-internal/public_html ev "if (function_exists('valgahalla_synch_node_export_find_orphaned_fc_items')) { print_r(count(valgahalla_synch_node_export_find_orphaned_fc_items()));}" -y >> $FILE 2>&1
$DRUSH @sites --root=/var/www/valghalla-external/public_html ev "if (function_exists('valgahalla_synch_node_export_find_orphaned_fc_items')) { print_r(count(valgahalla_synch_node_export_find_orphaned_fc_items()));}" -y >> $FILE 2>&1

mail -s "Valghalla health check test" $SENDTO -a "From: Valghalla <domain@bellcom.dk>" < $FILE
