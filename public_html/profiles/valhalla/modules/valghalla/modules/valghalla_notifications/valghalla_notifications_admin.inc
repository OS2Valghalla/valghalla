<?php

/**
 * @file Valghalla notification admin extension.
 */

/**
 * Notification settings form definition.
 */
function valghalla_notifications_settings($form, $form_state, $election_nid = FALSE) {
  drupal_add_css(drupal_get_path('module', 'valghalla_notifications') . '/css/valghalla_notifications.css');
  $election = node_load($election_nid);

  $form['header'] = array(
    '#type' => 'item',
    '#markup' => '<h2>Redigerer notifications skabeloner for:<em> ' . $election->title . '</em></h2>',
  );

  $types = valghalla_notifications_get_types();

  // Load all volunteer and template role nodes.
  $role_nodes = node_load_multiple(array(), array('type' => 'roles'));

  $form['valghalla_volunteers_notifications']['notifications'] = array(
    '#type' => 'fieldset',
    '#title' => t('Standard notifications skabeloner'),
    '#description' => '<a href="/valghalla/administration/notifications/add?destination=' . current_path() . '"><span class="glyphicon glyphicon-plus"></span> ' . t('Tilføj ny') . '</a>',
  );
  $gateways_list = valghalla_notifications_gateways();
  $gateway_options = array('-1' => t('None'));
  foreach ($gateways_list as $gateway_id => $gateway) {
    $gateway_options[$gateway_id] = $gateway['name'];
  }
  // Generate form for selecting general notifications.
  foreach ($types as $type => $type_title) {
    $form['valghalla_volunteers_notifications']['notifications'][$type] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('notifications')),
    );
    $type_variable_name = 'valghalla_notifications_' . $election_nid . '_' . $type . '_gateway_id';
    $type_value = variable_get($type_variable_name, '-1');
    $form['valghalla_volunteers_notifications']['notifications'][$type][$type_variable_name] = array(
      '#type' => 'radios',
      '#title' => $type_title,
      '#options' => $gateway_options,
      '#default_value' => $type_value,
    );
    foreach ($gateways_list as $gateway_id => $gateway) {
      $options = valghalla_notifications_generate_options($gateway_id);
      $description = t('Vælg @gateway_name skabelon for @type_title', array(
        '@gateway_name' => $gateway['name'],
        '@type_title' => $type_title,
      ));
      $variable_name = 'valghalla_notifications_'  . $election_nid . '_' . $type . '_' . $gateway_id;
      $tpl_nid = variable_get($variable_name, FALSE);

      if ($tpl_nid) {
        $description .= ' <a class="form-submit" href="/node/' . $tpl_nid . '/edit?destination=' . current_path() . '">' . t('edit') . '</a>';
      }
      else {
        $tpl_nid = 1;
      }

      $form['valghalla_volunteers_notifications']['notifications'][$type][$variable_name] = array(
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => $tpl_nid,
        '#description' => $description,
        '#states' => array(
          'visible' => array(
            ':input[name="' . $type_variable_name . '"]' => array('value' => $gateway_id),
          ),
        ),
      );
    }
  }

  // Role specific notifications.
  // Generate fieldset for each role.
  $form['valghalla_role_notifications'] = array(
    '#title' => t('Rollespecifikke @gateway_name skabeloner', array('@gateway_name' => $gateway['name'])),
    '#type' => 'fieldset',
    '#description' => t('Herunder kan du vælge rollespecifikke @gateway_name skabeloner. Disse indstillinger overskriver "Standard @gateway_name skabeloner" for den valgte rolle.',
      array('@gateway_name' => $gateway['name'])
    ),
  );

  // Override 'none' for role specific notifications.
  // Let the user know that the default role value is inherited.
  foreach ($role_nodes as $role_node) {
    $form['valghalla_role_notifications'][$role_node->nid] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => $role_node->title,
    );

    // Show referenced notifications for each role.
    foreach ($types as $type => $type_title) {
      $form['valghalla_role_notifications'][$role_node->nid][$type] = array(
        '#type' => 'container',
        '#attributes' => array('class' => array('notifications')),
      );
      foreach ($gateways_list as $gateway_id => $gateway) {
        $type_variable_name = 'valghalla_notifications_' . $election_nid . '_' . $role_node->nid . '_' . $type . '_gateway_id';
        $type_value = variable_get($type_variable_name, '');
        $form['valghalla_role_notifications'][$role_node->nid][$type][$type_variable_name] = array(
          '#type' => 'radios',
          '#title' => $type_title,
          '#options' => ['' => t('Nedarves standard')] + $gateway_options,
          '#default_value' => $type_value,
        );

        $options = valghalla_notifications_generate_options($gateway_id);

        $tpl_title = '';
        $variable_name = 'valghalla_notifications_' . $election_nid . '_' . $role_node->nid . '_' . $type . '_' . $gateway_id;
        $tpl_nid = variable_get($variable_name, 0);

        $tpl_node = node_load($tpl_nid);
        // Check if template node is valid.
        if (is_object($tpl_node)) {
          $tpl_title = $tpl_node->title;
        }

        // Show notification title, and edit link if a notification is chosen.
        $description = t('standard');
        if ($tpl_nid) {
          $description = $tpl_title . ': <a href="/node/' . $tpl_nid . '/edit?destination=' . current_path() . '">' . t('edit') . '</a>';
        }

        $form['valghalla_role_notifications'][$role_node->nid][$type][$variable_name] = array(
          '#type' => 'select',
          '#options' => $options,
          '#default_value' => $tpl_nid,
          '#description' => $description,
          '#states' => array(
            'visible' => array(
              ':input[name="' . $type_variable_name . '"]' => array('value' => $gateway_id),
            ),
          ),
        );
      }
    }
  }

  return system_settings_form($form);
}


/**
 * General mail settings.
 */
function valghalla_notifications_admin_settings() {
  // @TODO.
  // Cancellation from participants mail settings.
  $roles = db_query('SELECT name, rid FROM {role} WHERE rid > 2');
  $form['valghalla_volunteers_mail']['valghalla_notifications_cancel'] = array(
    '#type' => 'fieldset',
    '#title' => t('Besked ved afbud fra deltagere'),
    '#collapsible' => TRUE,
    '#description' => t('Vælg roller der skal modtage mails ved afbud. (Mails sendes som Bcc)'),
  );

  foreach ($roles as $role) {
    if ($role->name == 'Valgsekretær') {
      $form['valghalla_volunteers_mail']['valghalla_notifications_cancel']['valghalla_notifications_cancellation_from_participants_' . $role->rid] = array(
        '#type' => 'checkbox',
        '#title' => $role->name,
        '#default_value' => variable_get('valghalla_notifications_cancellation_from_participants_' . $role->rid),
      );
    }
  }

  $other_mails = variable_get('valghalla_notifications_cancellation_from_participants_to_mails');
  $other_check = variable_get('valghalla_notifications_cancellation_from_participants_others');

  if (!empty($other_mails) && empty($other_check)) {
    variable_set('valghalla_notifications_cancellation_from_participants_others', 0);
    variable_set('valghalla_notifications_cancellation_from_participants_to_mails', '');
  }
  elseif (empty($other_mails) && $other_check) {
    variable_set('valghalla_notifications_cancellation_from_participants_others', 0);
  }

  $form['valghalla_volunteers_mail']['valghalla_notifications_cancel']['valghalla_notifications_cancellation_from_participants_others'] = array(
    '#type' => 'checkbox',
    '#title' => t('Andre (udfyld tekstfeltet herunder):'),
    '#default_value' => variable_get('valghalla_notifications_cancellation_from_participants_others'),
  );
  $form['valghalla_volunteers_mail']['valghalla_notifications_cancel']['valghalla_notifications_cancellation_from_participants_to_mails'] = array(
    '#type' => 'textfield',
    '#description' => t('Liste af E-mailadresser adskildt af komma'),
    '#default_value' => variable_get('valghalla_notifications_cancellation_from_participants_to_mails'),
  );

  // Reminder settings.
  $form['valghalla_volunteers_mail']['options']['valghalla_rsvp_reminder_days'] = array(
    '#type' => 'textfield',
    '#title' => t('Påmindelses dage'),
    '#description' => t('Liste af dage før valg der skal udsendes påmindelser, adskildt af komma ( , ). f.eks. 14, 7, 5, 3'),
    '#default_value' => variable_get('valghalla_rsvp_reminder_days', '14, 7, 5, 3'),
  );

  $form['valghalla_volunteers_mail']['mails']['valghalla_notifications_send_invitation_on_add'] = array(
    '#title' => t('Send besked(invitation) til deltager når denne tilføjes på et valgsted.'),
    '#default_value' => variable_get('valghalla_notification_send_invitation_on_add', TRUE),
    '#type' => 'checkbox',
  );

  return system_settings_form($form);
}

/**
 * Extra validation needed to verify reminder days can be safely exloded.
 */
function valghalla_notifications_settings_validate(&$elements, &$form_state, $form_id = NULL) {
  if (preg_match('/[^\d|,| ]/', $form_state['values']['valghalla_rsvp_reminder_days'])) {
    // Found illigal chars in the form.
    form_set_error('valghalla_rsvp_reminder_days', 'Dette felt skal være en komma-separeret liste af hele tal.');
  }
}