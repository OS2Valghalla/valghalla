<?php

/**
 * @file
 * Handles the form submission of the valghalla_notifications_send_message_form.
 */

/**
 * Handles submitted form depending on what stage has been completed.
 *
 * @see valghalla_notifications_send_message_form()
 */
function valghalla_notifications_send_message_form_submit($form, &$form_state) {
  switch ($form_state['storage']['stage']) {
    case 'send_confirm':
      $form_state['multistep_values'][$form_state['storage']['stage']] = $form_state['values'];
      if ($form_state['triggering_element']['#value'] != 'Tilbage') {
        valghalla_notifications_send_confirm_submit($form, $form_state);
        $form_state['complete'] = TRUE;
      }
      break;

    default:
      $form_state['multistep_values'][$form_state['storage']['stage']] = $form_state['values'];
      $form_state['storage']['new_stage'] = valghalla_notifications_send_next_stage($form, $form_state);
      break;
  }

  if ($form_state['triggering_element']['#value'] == 'Tilbage') {
    $form_state['storage']['new_stage'] = valghalla_notifications_send_previous_stage($form, $form_state);
  }

  if (isset($form_state['multistep_values']['form_build_id'])) {
    $form_state['values']['form_build_id'] = $form_state['multistep_values']['form_build_id'];
  }
  $form_state['multistep_values']['form_build_id'] = $form_state['values']['form_build_id'];
  $form_state['storage']['stage'] = $form_state['storage']['new_stage'];
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit handler for send invitation.
 */
function valghalla_notifications_send_invitation_form_submit($form, &$form_state) {
  $multistep_values = $form_state['multistep_values'];

  // If the user is sending invitations and clicks back, send to the overview
  // page.
  if (isset($multistep_values['get_recipients']['invitation'])
    && $form_state['clicked_button']['#array_parents'][0] == 'back') {
    drupal_goto('valghalla/administration/valg');
  }

  $election_node = node_load($multistep_values['get_recipients']['election_nid']);
  if (is_object($election_node)) {
    $election_node->field_election_mails_send[$election_node->language][0]['value'] = 1;
    node_save($election_node);
  }

  valghalla_notifications_send_confirm_submit($form, $form_state);
}

/**
 * Handles the submission of the final stage.
 *
 * Step 3 : Send messages.
 */
function valghalla_notifications_send_confirm_submit($form, &$form_state) {
  $multistep_values = $form_state['multistep_values'];
  $volunteers_nids = valghalla_notifications_get_recipients($multistep_values['get_recipients']);
  $chunks = array_chunk($volunteers_nids, 10);

  foreach ($chunks as $batch) {
    $operations[] = array(
      'valghalla_notifications_send_batch', array(
        $batch,
        $form_state,
      ),
    );
  }

  batch_set(
    array(
      'title' => t('Sender beskeder'),
      'operations' => $operations,
      'finished' => 'valghalla_notifications_send_batch_finished',
      'progress_message' => t('Sender blok @current ud af @total. Hver blok indeholder (op til) 10 modtagere.'),
      'file' => drupal_get_path('module', 'valghalla_notifications') . '/includes/valghalla_notifications_submit.inc',
    )
  );
}

/**
 * Batch operation callback.
 *
 * @param array $recipients
 *   Array with recipients.
 * @param array $form_state
 *   Formt state array.
 * @param array $context
 *   Context array.
 */
function valghalla_notifications_send_batch(array $recipients, array $form_state, array &$context) {
  $notification_type = 'On demand';
  $multistep_values = $form_state['multistep_values'];
  $init_template_nid = $multistep_values['get_recipients']['valghalla_notifications_message_node_ID'];
  if (empty($context['results'])) {
    $context['results'] = array(
      'success' => array(),
      'fails' => array(),
    );
  }
  $gateway = valghalla_notifications_gateway($form_state['storage']['gateway_id']);

  foreach ($recipients as $nid => $fc_id) {
    $volunteer_fc = entity_load_single('field_collection_item', $fc_id);
    $volunteer = $volunteer_fc->hostEntity();
    if (!is_object($volunteer_fc)) {
      watchdog('valghalla_notifications', 'Volunteer entity is empty for id %fc_id ', ['fc_id' => $fc_id]);
      continue;
    }
    $tokens = _valghalla_helper_get_token_values($volunteer_fc);
    $template_nid = $init_template_nid;

    if ($template_nid == 'invite') {
      $notification_type = 'invitation';
      $election_nid = $multistep_values['get_recipients']['election_nid'];
      $gateway = valghalla_notifications_get_gateway($election_nid, $tokens['!position_nid'], 'invitation');
      $template_nid = valghalla_notifications_get_template($gateway['id'], $election_nid, $tokens['!position_nid'], $notification_type);
    }

    if (!valghalla_notifications_notification_allowed($gateway['id'], $volunteer)) {
      watchdog('valghalla_notifications', 'Notification has not send. Volunteer is excused from %gateway.', array(
        '%gateway' => $gateway['id'],
      ));
      $context['results']['fails'][] = $volunteer->title;
      continue;
    }

    $placeholders = array(
      '%nid' => $template_nid,
      '%name' => $tokens['!name'],
      '%gateway' => $gateway['name'],
    );

    $params = valghalla_notifications_get_message_by_nid($template_nid);
    if (empty($params)) {
      watchdog('valghalla_notifications', 'Template not found to send message to volunteer %name via %gateway', $placeholders);
      $context['results']['fails'][] = $volunteer->title;
      continue;
    }

    $params['notification_type'] = $notification_type;
    $params['volunteer_nid'] = $volunteer->nid;

    $params = array_merge(array('tokens' => $tokens), $params);
    $to = call_user_func($gateway['send_to_callback'], $volunteer);
    if (empty($to)) {
      watchdog('valghalla_notifications', 'Empty email for recipient with name %name', array('%name' => $params['tokens']['!name']), WATCHDOG_ERROR);
      $context['results']['fails'][] = $volunteer->title;
      continue;
    }

    if (call_user_func($gateway['send_message_callback'], $to, $params, $multistep_values)) {
      watchdog('valghalla_notifications', 'Message nid = %nid has been sent to volunteer %name via %gateway', $placeholders);
      $context['results']['success'][] = $volunteer->title;
    }
    else {
      watchdog('valghalla_notifications', 'Message nid = %nid has NOT been sent to volunteer %name via %gateway', $placeholders, WATCHDOG_ERROR);
      $context['results']['fails'][] = $volunteer->title;
    }
  }

  $context['message'] = t('Det er vigtigt at du ikke forlader denne side!');
  sleep(variable_get('valghalla_notifications_email_batch_sending_sleep_period',0));
}

/**
 * Batch finished callback.
 */
function valghalla_notifications_send_batch_finished($success, $results, $operations) {
  if ($success) {
    if ($results['success']) {
      drupal_set_message(format_plural(count($results['success']), 'Besked er blevet sendt.', '@count beskeder er blevet sendt.') . theme('item_list', array('items' => $results['success'])));
    }
    if ($results['fails']) {
      drupal_set_message(format_plural(count($results['fails']), 'Besked mislykkedes.', '@count beskeder mislykkedes.') . theme('item_list', array('items' => $results['fails'])), 'warning');
    }
    drupal_goto('valghalla/administration/valg');
  }
}
