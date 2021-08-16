<?php

/**
 * @file
 * Valghalla_external_server_signup.pages.inc.
 */

/**
 * Builds signup form.
 */
function valghalla_external_server_signup_signup_form($form, &$form_state) {
  // Getting election from state or URL.
  $election = isset($form_state['election']) ? $form_state['election'] : NULL;
  if (!$election) {
    $election = _valghalla_external_server_signup_fetch_entity_from_url('election', 'node');
    $form_state['election'] = $election;
  }

  // Getting election notification config.
  $field_election_notif_conf_export = field_get_items('node', $election, 'field_election_notif_conf_export');
  $form_state['election_notif_conf_export'] = unserialize($field_election_notif_conf_export[0]['value']);

  // Setting the election nid into the session,
  // as some of the functionality depends on it.
  $_SESSION['election_nid'] = $election->nid;

  // Getting party from state or URL.
  $party = isset($form_state['party']) ? $form_state['party'] : NULL;
  if (!$party) {
    $party = _valghalla_external_server_signup_fetch_entity_from_url('party', 'taxonomy_term');
    $form_state['party'] = $party;
  }

  if (!$election || !$party) {
    valghalla_external_server_signup_terminate_signup();
  }

  // Check is election is active.
  $field_language = field_language('node', $election, 'field_election_status');
  if (empty($election->field_election_status[$field_language][0]['value'])
    || empty($election->status)) {
    valghalla_external_server_signup_terminate_signup(1);
  }

  // Getting role from form state.
  $role = isset($form_state['role']) ? $form_state['role'] : NULL;
  if (!$role) {
    // Getting role from values.
    if (isset($form_state['values']) && isset($form_state['values']['role_select'])) {
      $role_selected = $form_state['values']['role_select'];
      $role = node_load($role_selected);
    }
    // Getting role from URL.
    else {
      $role = _valghalla_external_server_signup_fetch_entity_from_url('role', 'node');
      if ($role) {
        $form_state['role_in_url'] = TRUE;
        $form_state['role'] = $role;
      }
    }
  }
  // Getting polling station from form state.
  $polling_station = isset($form_state['polling_station']) ? $form_state['polling_station'] : NULL;
  if (!$polling_station) {
    // Getting polling station from values.
    if (isset($form_state['values']) && isset($form_state['values']['polling_station_select'])) {
      $polling_station_selected = $form_state['values']['polling_station_select'];
      $polling_station = node_load($polling_station_selected);
    }
    // Getting polling station from URL.
    else {
      $polling_station = _valghalla_external_server_signup_fetch_entity_from_url('pol_st', 'node');
      if ($polling_station) {
        $form_state['polling_station_in_url'] = TRUE;
        $form_state['polling_station'] = $polling_station;
      }
    }
  }

  drupal_set_title($election->title);

  // Getting form mode.
  $form_mode = valghalla_external_server_signup_get_form_mode($form_state);
  if (!$form_mode) {
    valghalla_external_server_signup_terminate_signup();
  }

  $cprProvider = variable_get('valghalla_external_server_signup_cpr_provider', 'nemlogin');

  if ($cprProvider == 'nemlogin') {
    // Attempt to fetch CPR from session.
    if (module_exists('nemlogin')) {
      $nemlogin_object = nemlogin_get_plugin_object();
      $cpr = !empty($nemlogin_object) ? $nemlogin_object->fetchValue('cpr') : NULL;
      // Adding dash, if missing. E.g. 123456-7890.
      if (preg_match('/[0-9]{10}/', $cpr)) {
        $cpr = substr($cpr, 0, 6) . '-' . substr($cpr, 6, 4);
      }
    }

    // Attempt to fetch CPR from form state.
    if (!$cpr && isset($form_state['input'])) {
      if (isset($form_state['input']['cpr'])) {
        $cpr = $form_state['input']['cpr'];
      }
    }

    $form['cpr'] = array(
      '#type' => 'hidden',
      '#default_value' => $cpr,
    );

    if (!$cpr && module_exists('nemlogin')) {
      $form['nemid_login_link'] = array(
        '#markup' => nemlogin_link('Login med Nemlogin', NULL, array(
          'attributes' => array(
            'class' => array(
              'btn',
              'btn-secondary',
            ),
          ),
        )),
      );
    }
    else {
      $form['#cpr'] = $cpr;
    }
  }
  else {
    $form['cpr'] = array(
      '#type' => 'textfield',
      '#description' => t('CPR'),
      '#attributes' => array(
        'placeholder' => t('CPR'),
      ),
      '#required' => TRUE,
    );
  }

  $form['email-container'] = array(
    '#prefix' => '<div id="js-email-container" class="form-group">',
    '#suffix' => '</div>',
  );
  $form['email-container']['email'] = array(
    '#type' => 'textfield',
    '#description' => t('Email'),
    '#attributes' => array(
      'placeholder' => t('Email'),
    ),
    '#required' => TRUE,
    '#access' => valghalla_external_server_signup_field_visibility($form_state, 'mail', ($role) ? $role->uuid : NULL),
  );
  $form['phone-container'] = array(
    '#prefix' => '<div id="js-phone-container" class="form-group">',
    '#suffix' => '</div>',
  );
  $form['phone-container']['phone'] = array(
    '#type' => 'textfield',
    '#description' => t('Telefon nr.'),
    '#attributes' => array(
      'placeholder' => t('Telefon nr.'),
    ),
    '#required' => TRUE,
    '#access' => valghalla_external_server_signup_field_visibility($form_state, 'sms', ($role) ? $role->uuid : NULL),
  );

  $field_party_subsc_accept_comment = field_get_items('taxonomy_term', $party, 'field_party_subsc_accept_comment');

  $form['comment'] = array(
    '#title' => t('Kommentar'),
    '#type' => 'textarea',
    '#rows' => 5,
    '#attributes' => array(
      'placeholder' => t('Skriv evt. en kommentar her'),
    ),
    '#access' => !empty($field_party_subsc_accept_comment) ? $field_party_subsc_accept_comment[0]['value'] : FALSE,
  );

  // Building a form.
  if (!isset($form_state['role_in_url'])) {
    if ($polling_station) {
      $available_roles = _valghalla_external_server_signup_get_available_roles($election->nid, $party->tid, $polling_station->nid);
    }
    else {
      $available_roles = _valghalla_external_server_signup_get_available_roles($election->nid, $party->tid);
    }

    $form_state['available_roles'] = $available_roles;

    // Role selector.
    $form['role_select'] = array(
      '#title' => t('Vælg rolle'),
      '#type' => 'select',
      '#options' => $available_roles,
      '#empty_option' => ('Vælg rolle'),
      '#empty_value' => 0,
      '#prefix' => '<div id="js-roles">',
      '#suffix' => '</div>',
      '#ajax' => array(
        'callback' => 'valghalla_external_server_signup_role_select_callback',
        'effect' => 'none',
      ),
    );

    $form['role_details'] = array(
      '#prefix' => '<div id="js-role-details">',
      '#suffix' => '</div>',
    );

    if (isset($form_state['values']) && isset($form_state['values']['role_select'])) {
      $role_selected = $form_state['values']['role_select'];
      $role = node_load($role_selected);
      $form_state['role'] = $role;

      if ($role) {
        $form['role_details']['#markup'] = _valghalla_external_server_signup_get_role_detail_html($role);
      }
    }
  }

  if (!isset($form_state['polling_station_in_url'])) {
    if ($role) {
      $available_polling_stations = _valghalla_external_server_signup_get_available_polling_stations($election->nid, $party->tid, $role->nid);
    }
    else {
      $available_polling_stations = _valghalla_external_server_signup_get_available_polling_stations($election->nid, $party->tid);
    }

    $form_state['available_polling_stations'] = $available_polling_stations;

    $form['polling_station_select'] = array(
      '#title' => t('Vælg valgsted'),
      '#type' => 'select',
      '#options' => $available_polling_stations,
      '#empty_option' => t('Vælg valgsted'),
      '#empty_value' => 0,
      '#prefix' => '<div id="js-polling-stations">',
      '#suffix' => '</div>',
      '#ajax' => array(
        'callback' => 'valghalla_external_server_signup_polling_station_select_callback',
        'effect' => 'none',
      ),
    );

    if (isset($form_state['values']) && isset($form_state['values']['polling_station_select'])) {
      $polling_station_selected = $form_state['values']['polling_station_select'];
      $polling_station = node_load($polling_station_selected);
    }
  }

  $polling_station_details_markup = '';
  if ($polling_station) {
    $polling_station_details_markup = _valghalla_external_server_signup_get_polling_station_detail_html($polling_station);
  }

  if ($polling_station_details_markup === '') {
    $form['polling_station_details'] = array(
      '#prefix' => '<div id="js-polling-station-details" class="no-content">',
      '#suffix' => '</div>',
    );
  }
  else {
    $form['polling_station_details'] = array(
      '#prefix' => '<div id="js-polling-station-details" class="form-group">',
      '#suffix' => '</div>',
      '#markup' => $polling_station_details_markup,
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Tilmeld'),
  );

  // Determining if we need to show consent checkbox.
  $terms_agreement_text = '';
  $field_election_disable_consent = field_get_items('node', $election, 'field_election_disable_consent');

  // If field is empty, or the value is 0.
  if (empty($field_election_disable_consent) || $field_election_disable_consent[0]['value'] == 0) {
    $field_election_consent_text = field_get_items('node', $election, 'field_election_consent_text');
    if (!empty($field_election_consent_text) && !empty($field_election_consent_text[0]['value'])) {
      $terms_agreement_text = $field_election_consent_text[0]['value'];
    }
    else {
      $terms_agreement_text = t('Accepter at vi gemmer dine oplysninger.');
    }
  }
  if (!empty($party->field_party_subsc_terms_text)) {
    $field_party_subsc_terms_text = field_get_items('taxonomy_term', $party, 'field_party_subsc_terms_text');
    $terms_agreement_text = $field_party_subsc_terms_text[0]['value'];
  }

  // We have some text, so consent checkbox needs to be displayed.
  if (!empty($terms_agreement_text)) {
    $form['terms_agreement'] = array(
      '#type' => 'checkbox',
      '#title' => $terms_agreement_text ,
      '#required' => TRUE,
    );

    // Adding ajax behaviour only if seat is available.
    $form['submit']['#states'] = array(
      'enabled' => array(
        ':input[name="terms_agreement"]' => array('checked' => TRUE),
      ),
    );
  }

  $form['no_seats_message'] = array(
    '#prefix' => '<div id="js-no-seats-message">',
    '#suffix' => '</div>',
    '#markup' => '',
  );

  // Checking if we have no seats available.
  $is_seat_available = valghalla_external_server_signup_form_is_seat_available($form_mode, $form_state);

  if (!$is_seat_available) {
    $form['no_seats_message']['#markup'] = _valghalla_external_server_signup_get_no_seats_message_html();
    $form['submit']['#disabled'] = 'disabled';

    $form['cpr']['#access'] = FALSE;
    $form['email-container']['#access'] = FALSE;
    $form['phone-container']['#access'] = FALSE;
    $form['comment']['#access'] = FALSE;
    $form['role_select']['#access'] = FALSE;
    $form['role_details']['#access'] = FALSE;
    $form['polling_station_select']['#access'] = FALSE;
    $form['polling_station_details']['#access'] = FALSE;
    $form['terms_agreement']['#access'] = FALSE;
    $form['submit']['#access'] = FALSE;
  }

  // Hiding fields if CPR is not present.
  if ($cprProvider == 'nemlogin' && !$cpr) {
    $form['cpr']['#access'] = FALSE;
    $form['email-container']['#access'] = FALSE;
    $form['phone-container']['#access'] = FALSE;
    $form['comment']['#access'] = FALSE;
    $form['role_select']['#access'] = FALSE;
    $form['polling_station_select']['#access'] = FALSE;
    $form['terms_agreement']['#access'] = FALSE;
    $form['submit']['#access'] = FALSE;
  }

  // Making the replacements.
  if ($form_mode) {
    $replacements = valghalla_external_server_signup_get_form_replacements($form_mode, $form_state);

    $form['#upper_text'] = variable_get('valghalla_external_server_signup_text_' . $form_mode . '_upper');
    $form['#bottom_text'] = variable_get('valghalla_external_server_signup_text_' . $form_mode . '_bottom');
    $tokens = array_keys(_valghalla_external_server_signup_text_get_replacement_tokens($form_mode));
    $form['#upper_text'] = str_replace($tokens, $replacements, $form['#upper_text']);
    $form['#bottom_text'] = str_replace($tokens, $replacements, $form['#bottom_text']);
  }
  if (!empty($party->field_party_subsc_message)) {
    $field_party_subsc_message = field_get_items('taxonomy_term', $party, 'field_party_subsc_message');
    $form['#party_message'] = $field_party_subsc_message[0]['value'];
  }

  $form['#theme'] = array('valghalla_external_server_signup_form');

  return $form;
}

/**
 * Ajax callback for role select.
 *
 * Updated the list of polling stations, and selected polling station details.
 *
 * @return mixed
 *   Polling station container element.
 */
function valghalla_external_server_signup_role_select_callback($form, &$form_state) {
  $commands = array();

  $role_selected = $form_state['values']['role_select'];
  $role = node_load($role_selected);

  // Updating the email and phone fields visibility.
  $form['email-container']['email']['#access'] = valghalla_external_server_signup_field_visibility($form_state, 'mail', $role->uuid);
  $commands[] = ajax_command_replace('#js-email-container', drupal_render($form['email-container']));

  $form['phone-container']['phone']['#access'] = valghalla_external_server_signup_field_visibility($form_state, 'sms', $role->uuid);
  $commands[] = ajax_command_replace('#js-phone-container', drupal_render($form['phone-container']));

  // Updating role details.
  $form['role_details']['#markup'] = ($role) ? _valghalla_external_server_signup_get_role_detail_html($role) : '';
  $commands[] = ajax_command_replace('#js-role-details', drupal_render($form['role_details']));

  if (!isset($form_state['polling_station_in_url'])) {
    $polling_station_selected = $form_state['values']['polling_station_select'];

    // Updating polling station select.
    if ($role_selected) {
      $psExists = FALSE;
      if ($polling_station_selected) {
        foreach ($form_state['available_polling_stations'] as $cons => $psList) {
          if (array_key_exists($polling_station_selected, $psList)) {
            $psExists = TRUE;
          }
        }
      }

      if (!$polling_station_selected || !$psExists) {
        $polling_station_selected = 0;
        $form['polling_station_select']['#value'] = $polling_station_selected;
      }
    }
    $commands[] = ajax_command_replace('#js-polling-stations', drupal_render($form['polling_station_select']));

    // Updating value in the form state.
    $form_state['values']['polling_station_select'] = $polling_station_selected;

    // Updating polling station details.
    if ($polling_station_selected) {
      $polling_station = node_load($polling_station_selected);
      $form['polling_station_details']['#markup'] = _valghalla_external_server_signup_get_polling_station_detail_html($polling_station);
    }
    else {
      $form['polling_station_details']['#markup'] = '';
    }

    $commands[] = ajax_command_replace('#js-polling-station-details', drupal_render($form['polling_station_details']));

    // Updating no seats message.
    $form['no_seats_message']['#markup'] = empty($form_state['available_polling_stations']) ? _valghalla_external_server_signup_get_no_seats_message_html() : '';
    $commands[] = ajax_command_replace('#js-no-seats-message', drupal_render($form['no_seats_message']));
  }

  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Ajax callback for polling station select.
 *
 * Updated the polling station details.
 *
 * @return mixed
 *   Polling station details element.
 */
function valghalla_external_server_signup_polling_station_select_callback($form, &$form_state) {
  $commands = array();

  $polling_station_selected = $form_state['values']['polling_station_select'];

  // Updating polling station details.
  if ($polling_station_selected) {
    $polling_station = node_load($polling_station_selected);
    $form['polling_station_details']['#markup'] = _valghalla_external_server_signup_get_polling_station_detail_html($polling_station);
  }
  else {
    $form['polling_station_details']['#markup'] = '';
  }
  $commands[] = ajax_command_replace('#js-polling-station-details', drupal_render($form['polling_station_details']));

  // Updating role select.
  if (!isset($form_state['role_in_url'])) {
    $role_selected = $form_state['values']['role_select'];
    if ($polling_station_selected) {
      if (!$role_selected || !array_key_exists($role_selected, $form_state['available_roles'])) {
        $role_selected = 0;
        $form['role_select']['#value'] = $role_selected;
      }
    }
    $commands[] = ajax_command_replace('#js-roles', drupal_render($form['role_select']));

    // Updating value in the form state.
    $form_state['values']['role_select'] = $role_selected;

    // Updating role details.
    $role = ($role_selected) ? node_load($role_selected) : NULL;
    $form['role_details']['#markup'] = ($role) ? _valghalla_external_server_signup_get_role_detail_html($role) : '';
    $commands[] = ajax_command_replace('#js-role-details', drupal_render($form['role_details']));

    // Updating no seats message.
    $form['no_seats_message']['#markup'] = empty($form_state['available_roles']) ? _valghalla_external_server_signup_get_no_seats_message_html() : '';
    $commands[] = ajax_command_replace('#js-no-seats-message', drupal_render($form['no_seats_message']));
  }

  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Helper function.
 *
 * Cancels the subscription by redirecting user to front page
 * with a warning message.
 */
function valghalla_external_server_signup_terminate_signup($terminate_code = 0) {
  // Throwing the warning and redirecting to the main page.
  $messages = array(
    0 => t('Tilmeldingslinket er ikke gyldigt eller er blevet deaktiveret.'),
    1 => t('Tilmelding til valget er i øjeblikket deaktiveret.'),
  );

  if (empty($messages[$terminate_code])) {
    $terminate_code = 0;
  }

  drupal_set_message($messages[$terminate_code], 'warning');
  drupal_goto();
}

/**
 * Validates the CPR in the form.
 *
 * Also changes the format of CPR, if needed.
 */
function valghalla_external_server_signup_signup_form_validate($form, &$form_state) {
  $cpr = $form_state['values']['cpr'];

  // Adding dash, if missing. E.g. 123456-7890.
  if (preg_match('/^[0-9]{10}$/', $cpr)) {
    $cpr = substr($cpr, 0, 6) . '-' . substr($cpr, 6, 4);
    $form_state['input']['cpr'] = $cpr;
  }
  if (!preg_match('/[0-9]{6}-[0-9]{4}/', $cpr)) {
    form_set_error('cpr', t('Forkert CPR-format, korrekt format er 123456-7890'));
  }

  if ($form['email-container']['email']['#access']) {
    $email = $form_state['values']['email'];
    if (!valid_email_address($email)) {
      form_set_error('email', t('Forkert email format'));
    }
  }

  if ($form['phone-container']['phone']['#access']) {
    $phone = $form_state['values']['phone'];
    if (empty($phone)) {
      form_set_error('phone', t('Telefonnummeret er tom'));
    }
    elseif (strlen($phone) > 12) {
      form_set_error('phone', t('Telefonnummeret er for langt'));
    }
  }

  if (isset($form_state['values']['polling_station_select']) && $form_state['values']['polling_station_select'] === 0) {
    form_set_error('polling_station_select', t('Felter %field er påkrævet', array('%field' => 'Valgsted')));
  }

  if (isset($form_state['values']['role_select']) && $form_state['values']['role_select'] === 0) {
    form_set_error('role_select', t('Felter %field  er påkrævet', array('%field' => 'Role')));
  }

  if (strcmp($form_state['triggering_element']['#type'], 'submit') === 0) {
    // Check place availability.
    $election_nid = $form_state['election']->nid;
    $party_tid = $form_state['party']->tid;
    $role_nid = isset($form_state['role']->nid) ? $form_state['role']->nid : $form_state['values']['role_select'];
    $polling_station_nid = isset($form_state['polling_station']) ? $form_state['polling_station']->nid : $form_state['values']['polling_station_select'];

    if ($election_nid && $party_tid && $role_nid && $polling_station_nid) {
      if (!_valghalla_external_server_signup_check_place_is_available($election_nid, $party_tid, $role_nid, $polling_station_nid)) {
        form_set_error('', t('Pladserne er desværre blevet optaget'));
      }
    }
  }
}

/**
 * Create node for volunteer and adding to volunteer to post.
 */
function valghalla_external_server_signup_signup_form_submit($form, &$form_state) {
  $election_nid = $form_state['election']->nid;
  $party_tid = $form_state['party']->tid;
  $role_nid = isset($form_state['role']->nid) ? $form_state['role']->nid : $form_state['values']['role_select'];
  $polling_station_nid = isset($form_state['polling_station']) ? $form_state['polling_station']->nid : $form_state['values']['polling_station_select'];

  if ($polling_station_nid && $role_nid && $party_tid && $election_nid) {
    // Encrypting CPR right away.
    $cpr = valghalla_synch_node_export_get_encrypt($form_state['values']['cpr']);

    $volunteer_node = odin_load_volunteer_by_cpr($cpr);

    if (!$volunteer_node) {
      $volunteer_node = new stdClass();
      $volunteer_node->type = 'volunteers';
      $volunteer_node->language = LANGUAGE_NONE;
      node_object_prepare($volunteer_node);
      $volunteer_node->title = "New volunteer, CPR: " . $cpr;
    }

    if (!isset($volunteer_node->nid) || !odin_volunteer_has_post_on_election($volunteer_node->nid, $election_nid)) {
      $volunteer_node->status = 1;
      $volunteer_node->field_cpr_number[LANGUAGE_NONE][]['value'] = $cpr;
      $volunteer_node->field_external_signup[LANGUAGE_NONE][]['value'] = 1;
      $volunteer_node->field_party[LANGUAGE_NONE][0]['tid'] = $party_tid;

      if (isset($form_state['values']['email'])) {
        $email_encrypted = valghalla_synch_node_export_get_encrypt($form_state['values']['email']);
        $volunteer_node->field_email[LANGUAGE_NONE][]['email'] = $email_encrypted;
      }
      if (isset($form_state['values']['phone'])) {
        $phone_encrypted = valghalla_synch_node_export_get_encrypt($form_state['values']['phone']);
        $volunteer_node->field_phone[LANGUAGE_NONE][]['value'] = $phone_encrypted;
      }

      node_save($volunteer_node);
      $post_info = valghalla_volunteers_add_volunteer_to_post($volunteer_node->nid, $polling_station_nid, $role_nid, $party_tid, $election_nid, 1);
      $post = entity_load('field_collection_item', array($post_info['fcid']));

      if ($post) {
        $post = reset($post);
        $post->field_rsvp_comment[LANGUAGE_NONE][]['value'] = $form_state['values']['comment'];
        $post->save();
      }

      // Send e-mail to email-address set on election.
      if (!empty($form_state['values']['comment'])) {
        _valghalla_volunteers_send_volunteer_comment($post, $form_state['values']['comment']);
      }

      $form_state['redirect'] = array('valghalla/external-signup-success');
      if (module_exists('nemlogin')) {
        $form_state['redirect'] = array(
          'nemlogin/logout',
          array(
            'query' => array(
              'destination' => 'valghalla/external-signup-success',
            ),
          ),
        );
      }
    }
    else {
      drupal_set_message(t('Personen er allerede tildelt en anden plads ved samme valg'), 'warning');
    }
  }
}

/**
 * External server signup settings form.
 */
function valghalla_external_server_signup_settings_form($form, $form_state) {
  $form['valghalla_external_server_signup_cpr_provider'] = array(
    '#type' => 'radios',
    '#title' => t('Volunteer CPR'),
    '#options' => array(
      'nemlogin' => t('Require via Nemlogin'),
      'manual' => t('Provide manually (extra field)')
    ),
    '#default_value' => variable_get('valghalla_external_server_signup_cpr_provider', 'nemid'),
    '#description' => t('Determine how the CPR of volunteer is collected'),
  );

  /* Party only START */
  $form['valghalla_external_server_signup_text_party_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Party only texts'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['valghalla_external_server_signup_text_party_fs']['valghalla_external_server_signup_text_party_upper'] = array(
    '#title' => t('Party only upper text'),
    '#type' => 'textarea',
    '#default_value' => variable_get('valghalla_external_server_signup_text_party_upper', '<p>Du kan her tilmelde dig til !election_name den !election_time for !party_name.</p>
<p>Log ind med NemID i nedenstående formular for at fortsætte.</p>'),
  );
  $form['valghalla_external_server_signup_text_party_fs']['valghalla_external_server_signup_text_party_bottom'] = array(
    '#title' => t('Party only bottom text'),
    '#type' => 'textarea',
    '#default_value' => variable_get('valghalla_external_server_signup_text_party_bottom', '<p>Ved at aktivere tilmeld-knappen herunder, accepterer du samtidigt at !municipality registrerer CPR nummer som tilmeldt på ovenstående afstemningssted. Du skal møde på ovenstående adresse d. !election_time.</p>
<p>Du vil herefter modtage en bekræftelse, her står der mere om dine opgaver på valgdagen, hvornår du skal møde samt ovenstående navn på valgsted og adresse.</p>'),
  );
  $token_str = array();
  foreach (_valghalla_external_server_signup_text_get_replacement_tokens('party') as $token => $text) {
    $token_str[] = "<li>$token - $text</li>";
  }
  $form['valghalla_external_server_signup_text_party_fs'][] = array(
    '#type' => 'fieldset',
    '#title' => t('Replacement'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    0 => array(
      '#markup' => '<ul>' . implode('', $token_str) . '</ul>',
    ),
  );
  /* Party only END */

  /* Party & role START */
  $form['valghalla_external_server_signup_text_party_role_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Party and role text'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['valghalla_external_server_signup_text_party_role_fs']['valghalla_external_server_signup_text_party_role_upper'] = array(
    '#title' => t('Party and role upper text'),
    '#type' => 'textarea',
    '#default_value' => variable_get('valghalla_external_server_signup_text_party_role_upper', '<p>Du kan her tilmelde dig til !election_name den !election_time for !party_name.</p>
<p>Du kan vælge mellem de valgsteder der har en ledig plads.</p>
<p>Log ind med NemID i nedenstående formular for at fortsætte.</p>'),
  );
  $form['valghalla_external_server_signup_text_party_role_fs']['valghalla_external_server_signup_text_party_role_bottom'] = array(
    '#title' => t('Party and role bottom text'),
    '#type' => 'textarea',
    '#default_value' => variable_get('valghalla_external_server_signup_text_party_role_bottom', '<p>Ved at aktivere tilmeld-knappen herunder, accepterer du samtidigt at !municipality registrerer CPR nummer som tilmeldt på ovenstående afstemningssted. Du skal møde på ovenstående adresse d. !election_time.</p>
<p>Du vil herefter modtage en bekræftelse, her står der mere om dine opgaver på valgdagen, hvornår du skal møde samt ovenstående navn på valgsted og adresse.</p>'),
  );
  $token_str = array();
  foreach (_valghalla_external_server_signup_text_get_replacement_tokens('party_role') as $token => $text) {
    $token_str[] = "<li>$token - $text</li>";
  }
  $form['valghalla_external_server_signup_text_party_role_fs'][] = array(
    '#type' => 'fieldset',
    '#title' => t('Replacement'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    0 => array(
      '#markup' => '<ul>' . implode('', $token_str) . '</ul>',
    ),
  );
  /* Party & role END */

  /* Party & polling station START */
  $form['valghalla_external_server_signup_text_party_polling_station_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Party and polling_station text'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['valghalla_external_server_signup_text_party_polling_station_fs']['valghalla_external_server_signup_text_party_polling_station_upper'] = array(
    '#title' => t('Party and polling station upper text'),
    '#type' => 'textarea',
    '#default_value' => variable_get('valghalla_external_server_signup_text_party_polling_station_upper', '<p>Du kan her tilmelde dig til !election_name den !election_time for !party_name.</p>
<p>Du kan vælge en rolle blandt de ledige pladser !party_name er tildelt på !polling_station_name.</p>
<p>Log ind med NemID i nedenstående formular for at fortsætte.</p>'),
  );
  $form['valghalla_external_server_signup_text_party_polling_station_fs']['valghalla_external_server_signup_text_party_polling_station_bottom'] = array(
    '#title' => t('Party and polling station bottom text'),
    '#type' => 'textarea',
    '#default_value' => variable_get('valghalla_external_server_signup_text_party_polling_station_bottom', '<p>Ved at aktivere tilmeld-knappen herunder, accepterer du samtidigt at !municipality registrerer CPR nummer som tilmeldt som !role_name på afstemningsstedet !polling_station_name. Du skal møde på ovenstående adresse d. !election_time !role_meeting_time.</p>
<p>Du vil herefter modtage en bekræftelse, her står der mere om dine opgaver på valgdagen, hvornår du skal møde samt ovenstående navn på valgsted og adresse.</p>'),
  );
  $token_str = array();
  foreach (_valghalla_external_server_signup_text_get_replacement_tokens('party_polling_station') as $token => $text) {
    $token_str[] = "<li>$token - $text</li>";
  }
  $form['valghalla_external_server_signup_text_party_polling_station_fs'][] = array(
    '#type' => 'fieldset',
    '#title' => t('Replacement'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    0 => array(
      '#markup' => '<ul>' . implode('', $token_str) . '</ul>',
    ),
  );
  /* Party & polling station END */

  /* Party, role & polling station START */
  $form['valghalla_external_server_signup_text_party_role_polling_station_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Party, role and polling station text'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['valghalla_external_server_signup_text_party_role_polling_station_fs']['valghalla_external_server_signup_text_party_role_polling_station_upper'] = array(
    '#title' => t('Party, role and polling station upper text'),
    '#type' => 'textarea',
    '#default_value' => variable_get('valghalla_external_server_signup_text_party_role_polling_station_upper', '<p>Du kan her tilmelde dig som !role_name på !polling_station_name ved !election_name den !election_time for !party_name.</p>
<p>Log ind med NemID i nedenstående formular for at fortsætte.</p>'),
  );
  $form['valghalla_external_server_signup_text_party_role_polling_station_fs']['valghalla_external_server_signup_text_party_role_polling_station_bottom'] = array(
    '#title' => t('Party, role and polling station bottom text'),
    '#type' => 'textarea',
    '#default_value' => variable_get('valghalla_external_server_signup_text_party_role_polling_station_bottom', '<p>Ved at aktivere tilmeld-knappen herunder, accepterer du samtidigt at !municipality registrerer CPR nummer som tilmeldt som !role_name på afstemningsstedet !polling_station_name. Du skal møde på ovenstående adresse d. !election_time !role_meeting_time.</p>
<p>Du vil herefter modtage en bekræftelse, her står der mere om dine opgaver på valgdagen, hvornår du skal møde samt ovenstående navn på valgsted og adresse.</p>'),
  );
  $token_str = array();
  foreach (_valghalla_external_server_signup_text_get_replacement_tokens('party_role_polling_station') as $token => $text) {
    $token_str[] = "<li>$token - $text</li>";
  }
  $form['valghalla_external_server_signup_text_party_role_polling_station_fs'][] = array(
    '#type' => 'fieldset',
    '#title' => t('Replacement'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    0 => array(
      '#markup' => '<ul>' . implode('', $token_str) . '</ul>',
    ),
  );
  /* Party, role & polling station END */

  /* Success page START */
  $form['valghalla_external_server_signup_success_page_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Success page text'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['valghalla_external_server_signup_success_page_fs']['valghalla_external_server_signup_success_page_body'] = array(
    '#title' => t('Success page text'),
    '#type' => 'textarea',
    '#default_value' => variable_get('valghalla_external_server_signup_success_page_body', '<p>Tak for din tilmelding.</p>
<p>Dine oplysninger bliver nu kontrolleret, og du vil modtage en bekræftelse på din tilmelding via
digital post.</p>'),
  );
  /* Success page END */

  return system_settings_form($form);
}

/**
 * Returns the visibility status for email or phone field.
 *
 * Used both on form rendering and the ajax call.
 *
 * @param mixed $form_state
 *   Form state.
 * @param string $propery_name
 *   Name of the property to check for.
 * @param string $role_uuid
 *   The UUID of the role, can be null.
 *
 * @return bool
 *   TRUE or FALSE.
 */
function valghalla_external_server_signup_field_visibility($form_state, $propery_name, $role_uuid = NULL) {
  // Checking election overwrites first.
  $election = $form_state['election'];

  if ($propery_name == 'mail') {
    $field_election_ext_require_email = field_get_items('node', $election, 'field_election_ext_require_email');
    if (!empty($field_election_ext_require_email) && $field_election_ext_require_email[0]['value'] == 1) {
      return TRUE;
    }
  }
  elseif ($propery_name == 'sms') {
    $field_election_ext_require_phone = field_get_items('node', $election, 'field_election_ext_require_phone');
    if (!empty($field_election_ext_require_phone) && $field_election_ext_require_phone[0]['value'] == 1) {
      return TRUE;
    }
  }

  // Checking notification export config.
  $conf_export = $form_state['election_notif_conf_export'];

  if ($conf_export) {
    if ($role_uuid && array_key_exists($role_uuid, $conf_export) && isset($conf_export[$role_uuid][$propery_name])) {
      return $conf_export[$role_uuid][$propery_name] === TRUE;
    }
    else {
      return $conf_export['general'][$propery_name] === TRUE;
    }
  }
  else {
    return TRUE;
  }
}

/**
 * External signup success page.
 */
function _external_signup_success_page() {
  drupal_set_title(t('Tilmeldt'));

  $output = variable_get('valghalla_external_server_signup_success_page_body', '<p>Tak for din tilmelding.</p>
<p>Dine oplysninger bliver nu kontrolleret, og du vil modtage en bekræftelse på din tilmelding via
digital post.</p>');

  return $output;
}

/**
 * Function to determine form mode.
 *
 * @param mixed $form_state
 *   State of the form.
 *
 * @return string
 *   Form mode - party|party_role|party_polling_station|
 *   party_role_polling_station|NULL.
 */
function valghalla_external_server_signup_get_form_mode($form_state) {
  $form_mode = NULL;

  if (!isset($form_state['role_in_url']) && !isset($form_state['polling_station_in_url'])) {
    $form_mode = 'party';
  }
  elseif (isset($form_state['role_in_url']) && !isset($form_state['polling_station_in_url'])) {
    $form_mode = 'party_role';
  }
  elseif (!isset($form_state['role_in_url']) && isset($form_state['polling_station_in_url'])) {
    $form_mode = 'party_polling_station';
  }
  elseif (isset($form_state['role_in_url']) && isset($form_state['polling_station_in_url'])) {
    $form_mode = 'party_role_polling_station';
  }

  return $form_mode;
}

/**
 * Combines the replacements based on form mode.
 *
 * @param string $form_mode
 *   Mode of the form.
 * @param mixed $form_state
 *   State of the form.
 *
 * @return array
 *   Array of replacements.
 */
function valghalla_external_server_signup_get_form_replacements($form_mode, $form_state) {
  $replacements = array();

  $field_date = field_get_items('node', $form_state['election'], 'field_date');
  $election_date_str = format_date(strtotime($field_date[0]['value']), 'custom', 'd. F Y');

  if ($form_mode == 'party') {
    $field_partinavn = field_get_items('taxonomy_term', $form_state['party'], 'field_partinavn');
    $replacements = array(
      odin_get_municipality(variable_get('valhalla_settings_municipality')),
      $form_state['election']->title,
      $election_date_str,
      $field_partinavn[0]['value'],
    );
  }
  elseif ($form_mode == 'party_role') {
    $field_partinavn = field_get_items('taxonomy_term', $form_state['party'], 'field_partinavn');
    $field_description = field_get_items('node', $form_state['role'], 'field_description');
    $field_meetingtime = field_get_items('node', $form_state['role'], 'field_meetingtime');

    $replacements = array(
      odin_get_municipality(variable_get('valhalla_settings_municipality')),
      $form_state['election']->title,
      $election_date_str,
      $field_partinavn[0]['value'],
      $field_description[0]['value'],
      $field_meetingtime[0]['value'],
    );
  }
  elseif ($form_mode == 'party_polling_station') {
    $field_partinavn = field_get_items('taxonomy_term', $form_state['party'], 'field_partinavn');

    $replacements = array(
      odin_get_municipality(variable_get('valhalla_settings_municipality')),
      $form_state['election']->title,
      $election_date_str,
      $field_partinavn[0]['value'],
      $form_state['polling_station']->title,
    );
  }
  elseif ($form_mode == 'party_role_polling_station') {
    $field_partinavn = field_get_items('taxonomy_term', $form_state['party'], 'field_partinavn');
    $field_description = field_get_items('node', $form_state['role'], 'field_description');
    $field_meetingtime = field_get_items('node', $form_state['role'], 'field_meetingtime');

    $replacements = array(
      odin_get_municipality(variable_get('valhalla_settings_municipality')),
      $form_state['election']->title,
      $election_date_str,
      $field_partinavn[0]['value'],
      $field_description[0]['value'],
      $field_meetingtime[0]['value'],
      $form_state['polling_station']->title,
    );
  }

  return $replacements;
}

/**
 * Checks if the seat is available for that form.
 *
 * @param string $form_mode
 *   Mode of the form.
 * @param mixed $form_state
 *   State of the form.
 *
 * @return bool
 *   True of false.
 */
function valghalla_external_server_signup_form_is_seat_available($form_mode, $form_state) {
  $is_seat_available = TRUE;

  // Checking if we have no seats available.
  switch ($form_mode) {
    case 'party':
      if (empty($form_state['available_roles']) && empty($form_state['available_polling_stations'])) {
        $is_seat_available = FALSE;
      }
      break;

    case 'party_role':
      if (empty($form_state['available_polling_stations'])) {
        $is_seat_available = FALSE;
      }
      break;

    case 'party_polling_station':
      if (empty($form_state['available_roles'])) {
        $is_seat_available = FALSE;
      }
      break;

    case 'party_role_polling_station':
      if (!_valghalla_external_server_signup_check_place_is_available($form_state['election']->nid, $form_state['party']->tid, $form_state['role']->nid, $form_state['polling_station']->nid)) {
        $is_seat_available = FALSE;
      }
      break;
  }

  return $is_seat_available;
}
