<?php

/**
 * @file
 * Valghalla_volunteer_validator.utils.inc.
 */

/**
 * Cron hook callback.
 *
 * Updates validation criteria status for all volunteers, in case entry for
 * volunteer is missing, or it has not been since today's midnight.
 */
function valghalla_volunteer_validator_update_all_validation_criteria_statuses() {
  // Load all volunteers.
  $query = new EntityFieldQuery();
  $result = $query->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'volunteers')
    ->execute();

  // Fetch validation criteria status for each volunteer.
  if (isset($result['node'])) {
    $volunteers = node_load_multiple(array_keys($result['node']));
    if (!empty($volunteers) && is_array($volunteers)) {
      foreach ($volunteers as $volunteer) {
        if ($old_criteria_status = valghalla_volunteer_validator_get_validation_criteria_status($volunteer->nid)) {
          // Only update volunteers the weren't updated more than 3 days ago.
          // That means, that under normal conditions, every volunteer will be
          // updated on a first cron run after midnight every 3 days.
          if ($old_criteria_status['timestamp'] > strtotime('-3days 00:00:00', time())) {
            continue;
          }
        }

        valghalla_volunteer_validator_update_validation_criteria_status($volunteer);
      }
    }
  }
}

/**
 * Updates validation criteria status per volunteer.
 *
 * @param mixed $node
 *   Volunteer node.
 */
function valghalla_volunteer_validator_update_validation_criteria_status($node) {
  if ($node->type == 'volunteers') {
    // Getting CPR.
    $node_wrapper = entity_metadata_wrapper('node', $node);
    $cpr = $node_wrapper->field_cpr_number->value();

    // Getting criteria status.
    $criteria_status = valghalla_volunteer_validator_query_service('get_validation_criteria_status', $cpr);

    // Removing empty values.
    $criteria_status = array_filter($criteria_status);

    if (!empty($criteria_status)) {
      // Making array flat.
      $criteria_status = array_reduce($criteria_status, 'array_merge', array());

      $data = array(
        'nid' => $node->nid,
        'validation_criteria_status' => serialize($criteria_status),
        'timestamp' => time(),
      );

      if (valghalla_volunteer_validator_get_validation_criteria_status($node->nid)) {
        db_update('valghalla_volunteer_validation_criteria_status')
          ->fields($data)
          ->condition('nid', $data['nid'])
          ->execute();
      }
      else {
        db_insert('valghalla_volunteer_validation_criteria_status')
          ->fields($data)
          ->execute();
      }
    }
  }
}

/**
 * Gets volunteer validation criteria status.
 *
 * @param int $nid
 *   Nid of the volunteer.
 *
 * @return mixed
 *   Array with the info like:
 *   array(
 *     'nid' => nid of the node,
 *     'validation_criteria' => array(
 *       'citizenship' => true/false,
 *       'is_alive' => true/false
 *       'municipality' => true/false
 *     ),
 *     'timestamp' => timestamp when inserted/updated
 *   ).
 */
function valghalla_volunteer_validator_get_validation_criteria_status($nid) {
  $query = db_select('valghalla_volunteer_validation_criteria_status', 'v')
    ->fields('v');

  $query->condition('nid', $nid);
  $result = $query->execute()->fetchAssoc();

  if (isset($result['validation_criteria'])) {
    $result['validation_criteria'] = unserialize($result['validation_criteria']);
  }

  return $result;
}

/**
 * Removes volunteer validation criteria status.
 *
 * @param int $nid
 *   Nid of the volunteer.
 */
function valghalla_volunteer_validator_remove_validation_criteria_status($nid) {
  db_delete('valghalla_volunteer_validation_criteria_status')
    ->condition('nid', $nid)
    ->execute();
}
