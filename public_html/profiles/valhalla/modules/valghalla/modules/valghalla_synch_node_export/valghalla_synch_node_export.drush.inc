<?php

/**
 * @file
 * valghalla_synch_node_export.drush.inc.
 */

/**
 * Implements hook_drush_command().
 */
function valghalla_synch_node_export_drush_command() {
  $items = array();
  $items['valghalla-fc-clean-orphaned'] = array(
    'callback' => 'valghalla_synch_node_export_delete_orphaned_fcs_init',
    'description' => 'Drush command initializes orphaned field collection items delete batch operation.',
    'aliases' => array('valg-fc-co'),
    'examples' => array(
      'Perform orphaned field collection items delete' => 'drush valghalla-fc-clean-orphaned',
      'Perform orphaned field collection items delete (alias)' => 'drush valg-fc-co',
    ),
  );

  return $items;
}
