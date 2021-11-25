<?php

/**
 * @file
 * Valghalla_internal_server.utils.inc.
 */

/**
 * Compares two volunteer nodes.
 *
 * Calculates new and updated election info between first and the second node.
 *
 * @param mixed $volunteer
 *   First volunteer node.
 * @param mixed $volunteer_revision
 *   Second volunteer node.
 *
 * @return array
 *   Array of election info field collections, mapped by uuid.
 */
function valghalla_internal_server_get_updated_seats($volunteer, $volunteer_revision) {
  $volunteer_seats = array();
  $volunteer_revision_seats = array();

  $field_electioninfo = field_get_items('node', $volunteer, 'field_electioninfo');
  if (!empty($field_electioninfo)) {
    foreach ($field_electioninfo as $election_info) {
      $item = entity_load_single('field_collection_item', $election_info['value']);
      $volunteer_seats[$item->uuid] = $item;
    }
  }

  if ($volunteer_revision) {
    $revision_field_electioninfo = field_get_items('node', $volunteer_revision, 'field_electioninfo');
    if (!empty($revision_field_electioninfo)) {
      foreach ($revision_field_electioninfo as $revision_election_info) {
        $revision_item = entity_load_single('field_collection_item', $revision_election_info['value']);

        // If field collection with the same uuid is already present in the
        // volunteer_revision, we check if RSVP field has been changed.
        // If yes - field collection has been updated, if not - we consider it
        // has not been changed.
        if (array_key_exists($revision_item->uuid, $volunteer_seats)) {

          $revision_field_rsvp = field_get_items('field_collection_item', $revision_item, 'field_rsvp');
          if (!empty($revision_field_rsvp)) {
            $revision_rsvp = $revision_field_rsvp[0]['value'];
          }

          $field_rsvp = field_get_items('field_collection_item', $volunteer_seats[$revision_item->uuid], 'field_rsvp');
          if (!empty($field_rsvp)) {
            $rsvp = $field_rsvp[0]['value'];
          }

          // RSVP field has been changed, don't add this seat to the list of
          // existing seats.
          if (strcmp($rsvp, $revision_rsvp) !== 0) {
            continue;
          }
        }
        $volunteer_revision_seats[$revision_item->uuid] = $revision_item;
      }
    }
  }

  if (!empty($volunteer_revision_seats)) {
    // Return all entries from $volunteer_seats that are not present in
    // $volunteer_revision_seats.
    return array_diff_key($volunteer_seats, $volunteer_revision_seats);
  }
  else {
    return $volunteer_seats;
  }
}

/**
 * Performs a validation of a single volunteer node.
 *
 * Also performs the seat validation.
 *
 * @param mixed $volunteer_node
 *   Volunteer node.
 * @param mixed $seat
 *   Seat to validate against - electionInfo field collection
 *
 * @return array
 *   Array with keys 'valid' - boolean, 'errors' - list of errors.
 */
function valghalla_internal_server_validate_volunteer_seat($volunteer_node, $seat) {
  $validationStatus = array(
    'valid' => TRUE,
    'errors' => array(),
  );

  $cpr_field = field_get_items('node', $volunteer_node, 'field_cpr_number');
  $cpr = $cpr_field[0]['value'];

  // Check formatting.
  if (!valghalla_volunteer_validator_validate_format($cpr)) {
    $validationStatus['errors'][] = t('Det angivne CPR-nummer er ikke korrekt formatteret (f.eks. 123456-8910).');
    $validationStatus['valid'] = FALSE;
  }

  // Check for duplicates.
  if ($nid = valghalla_volunteer_validator_check_duplicates($cpr)) {
    if ($nid !== $volunteer_node->nid) {
      $validationStatus['errors'][] = t('Der er allerede oprettet en deltager med det angivne CPR-nummer');
      $validationStatus['valid'] = FALSE;
    }
  }

  // Getting election.
  $election = NULL;
  $field_election = field_get_items('field_collection_item', $seat, 'field_election');
  if (!empty($field_election)) {
    $election_nid = $field_election[0]['target_id'];
    $election = node_load($election_nid);
  }

  // Getting party.
  $party = NULL;
  $field_post_party = field_get_items('field_collection_item', $seat, 'field_post_party');
  if (!empty($field_post_party)) {
    $party_tid = $field_post_party[0]['target_id'];
    $party = taxonomy_term_load($party_tid);
  }

  // Run CPR validation.
  $messages = _valghalla_volunteer_validator_cpr_validation($cpr, $election, $party);

  foreach ($messages as $message) {
    if ($message['status'] === FALSE) {
      $validationStatus['errors'][] = t('Det angivne CPR-nummer kan ikke valideres: @text', array('@text' => $message['text']));
      $validationStatus['valid'] = FALSE;
    }
  }

  return $validationStatus;
}

/**
 * Encodes term object as json string.
 *
 * @param mixed $term
 *   The term.
 *
 * @return string
 *   Encoded string.
 */
function valghalla_internal_server_term_to_json($term) {
  $voc = taxonomy_vocabulary_machine_name_load($term->vocabulary_machine_name);
  unset($term->vocabulary_machine_name);
  $term->vid = $voc->vid;

  return drupal_json_encode($term);
}

/**
 * Unsets the seat selected seat from the volunteer.
 *
 * @param mixed $seat
 *   Seat to unset.
 */
function valghalla_internal_server_volunteer_free_seat($seat) {
  $seat->delete();
}

/**
 * Generates the url for external signup.
 *
 * @param mixed $election
 *   Election node.
 * @param mixed $party
 *   Party taxonomy term.
 * @param mixed $role
 *   Role node, optional.
 * @param mixed $polling_station
 *   Polling station node, optional.
 *
 * @return string
 *   Generated subscribe url.
 */
function valghalla_internal_server_generate_external_subscribe_url($election, $party, $role = NULL, $polling_station = NULL) {
  $subscribe_url = '';
  if (!empty($election) && $external_service_url = valghalla_internal_server_get_external_server_url()) {
    // External subscription allowed only for active election.
    $field_status = field_get_items('node', $election, 'field_election_status');

    if (!empty($field_status[0]['value']) && !empty($election->status) && $party) {
      // Adding election and party, mandatory params.
      $subscribe_url = $external_service_url . '/valghalla/external-signup?election=' . $election->uuid . '&party=' . $party->uuid;

      if ($role) {
        // Adding role.
        $subscribe_url .= '&role=' . $role->uuid;
      }
      if ($polling_station) {
        // Adding polling station.
        $subscribe_url .= '&pol_st=' . $polling_station->uuid;
      }
    }
  }

  return $subscribe_url;
}

/**
 * Gets an external server URL without any path, also adds port if present.
 *
 * Does not have a leading slash.
 *
 * @return string
 *   External server URL.
 *   E.g. http://valghalla.dk
 */
function valghalla_internal_server_get_external_server_url() {
  $external_server_url = '';
  if ($external_service_endpoint = variable_get('valghalla_external_server_endpoint')) {
    // Parsing our endpoint to remove any paths.
    $url_parts = parse_url($external_service_endpoint);
    $external_server_url = $url_parts['scheme'] . '://' . $url_parts['host'] . (isset($url_parts['port']) ? ':' . $url_parts['port'] : '');
  }

  return $external_server_url;
}

/**
 * Deanonymizes the node.
 *
 * Does that by decrypting the CPR, email and phone fields.
 *
 * @param mixed $node
 *   The node to deanonymize.
 */
function valghalla_internal_server_deanonymize_node($node) {
  if ($node->type == 'volunteers') {
    $external_server_hash_salt = variable_get('valghalla_external_server_hash_salt');

    $cpr_field_language = field_language('node', $node, 'field_cpr_number');
    $cpr = $node->field_cpr_number[$cpr_field_language][0]['value'];

    // If CPR format fails, try decrypting it.
    if (!preg_match('/[0-9]{10}/', $cpr) && !preg_match('/[0-9]{6}-[0-9]{4}/', $cpr)) {
      $cpr = valghalla_synch_node_export_get_decrypt($cpr, $external_server_hash_salt);

      // Check the CPR format, after decryption.
      if (preg_match('/[0-9]{10}/', $cpr) || preg_match('/[0-9]{6}-[0-9]{4}/', $cpr)) {
        // Format passed, change the CPR in the node.
        $node->field_cpr_number[$cpr_field_language][0]['value'] = $cpr;
        $node->field_cpr_number[$cpr_field_language][0]['safe_value'] = $cpr;
      }
    }

    // Email field.
    $email_field_language = field_language('node', $node, 'field_email');
    if (isset($node->field_email[$email_field_language])) {
      $email = $node->field_email[$email_field_language][0]['email'];
      $email = valghalla_synch_node_export_get_decrypt($email, $external_server_hash_salt);
      $node->field_email[$email_field_language][0]['email'] = $email;
    }

    // Phone field.
    $phone_field_language = field_language('node', $node, 'field_phone');
    if (isset($node->field_phone[$phone_field_language])) {
      $phone = $node->field_phone[$phone_field_language][0]['value'];
      $phone = valghalla_synch_node_export_get_decrypt($phone, $external_server_hash_salt);
      $node->field_phone[$phone_field_language][0]['value'] = $phone;
    }
  }

  return $node;
}
