<?php

module_load_include('inc', 'os2logging', 'includes/os2logging.access_log');

/**
 * Valghalla TFA SMS Message report form.
 */
function valghalla_tfa_sms_message_report_form_year($form, &$form_state) {
  if (user_access('administer valghalla')) {
    $day_options = [];
    for ($i = 1; $i <= 31; $i++) {
      $day_options[$i] = $i;
    }

    $month_options = [];
    for ($i = 1; $i <= 12; $i++) {
      $month_options[$i] = $i;
    }

    $form['valghalla_tfa_year_period_date_day'] = [
      '#type' => 'select',
      '#title' => t('Periode start (dag)'),
      '#default_value' => variable_get('valghalla_tfa_year_period_date_day', 1),
      '#options' => $day_options,
      '#prefix' => '<div class="row form-group"><div class="col-md-3 col-sm-3">',
      '#suffix' => '</div>'
    ];
    $form['valghalla_tfa_year_period_date_month'] = [
      '#type' => 'select',
      '#title' => t('Periode start (måned)'),
      '#default_value' => variable_get('valghalla_tfa_year_period_date_month', 1),
      '#options' => $month_options,
      '#prefix' => '<div class="col-md-3 col-sm-6">',
      '#suffix' => '</div></div>'
    ];

    $form['submit'] = [
      '#type' => 'submit',
      '#value' => t('Beregn'),
      '#prefix' => '<div class="form-group">',
      '#suffix' => '</div>'
    ];
  }

  $header = array(
    'period' => array(
      'data' => t('Periode'),
      'type' => 'property',
    ),
    'Total' => array(
      'data' => t('I alt'),
      'type' => 'property',
    ),
  );


  // Start date or period.
  $date = new DateTime();
  $year = date("Y");
  $month = variable_get('valghalla_tfa_year_period_date_month', 1);
  $day = variable_get('valghalla_tfa_year_period_date_day', 1);

  // Start date should be e.g. 01/01/2021 00:00:00
  $date->setDate($year, $month, $day);
  $date->setTime(0,0);

  $startDateTs = $date->getTimestamp();
  $startDateHuman = $date->format("d/m/Y");

  // Adding 1 year and substracting 1 day.
  $date->add(new DateInterval("P1Y"));
  $date->sub(new DateInterval("P1D"));

  // End date should be e.g. 31/12/2021 23:59:59
  $date->setTime(23,59,59);
  $endDateTs = $date->getTimestamp();

  $endDateHuman = $date->format("d/m/Y");

  $query = db_select('valghalla_tfa_sms_message', 'sms');
  $query->fields('sms');
  $query->condition('sms.created', $startDateTs, '>=');
  $query->condition('sms.created', $endDateTs, '<=');
  $count = $query->execute()->rowCount();

  $options = array(
    array(
      "Fra $startDateHuman til $endDateHuman",
      $count
    )
  );

  $form[] = array(
    '#markup' => theme('table', array('header' => $header, 'rows' => $options))
  );

  return $form;
}

/**
 * Valghalla TFA SMS Message report form submit.
 */
function valghalla_tfa_sms_message_report_form_year_submit($form, &$form_state) {
  $values = $form_state['values'];
  variable_set('valghalla_tfa_year_period_date_day', $values['valghalla_tfa_year_period_date_day']);
  variable_set('valghalla_tfa_year_period_date_month', $values['valghalla_tfa_year_period_date_month']);
}

/**
 * Valghalla TFA SMS Message report form custom.
 */
function valghalla_tfa_sms_message_report_form_custom($form, &$form_state) {
  $day_options = array();
  for ($i = 1; $i <= 31; $i++) {
    $day_options[$i] = $i;
  }

  $month_options = array();
  for ($i = 1; $i <= 12; $i++) {
    $month_options[$i] = $i;
  }

  $form['valghalla_tfa_period_start_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Start dato'),
    "#date_format" => 'd/m/Y',
    '#default_value' => isset($_GET['start_date']) ? date_create_from_format('d/m/Y', $_GET['start_date'])->format('Y-m-d 00:00:00') : '',
    '#prefix' => '<div class="row form-group"><div class="col-md-4">',
    '#suffix' => '</div>'
  );
  $form['valghalla_tfa_period_end_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Slut dato'),
    "#date_format" => 'd/m/Y',
    '#default_value' => isset($_GET['end_date']) ? date_create_from_format('d/m/Y', $_GET['end_date'])->format('Y-m-d 23:59:59') : '',
    '#prefix' => '<div class="col-md-4">',
    '#suffix' => '</div></div>'
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Beregn'),
    '#prefix' => '<div class="form-group">',
    '#suffix' => '</div>'
  );

  $header = array(
    'period' => array(
      'data' => t('Periode'),
      'type' => 'property',
    ),
    'Total' => array(
      'data' => t('I alt'),
      'type' => 'property',
    ),
  );

  // Getting start date.
  if (isset($_GET['start_date'])) {
    $startDate = date_create_from_format('d/m/Y', $_GET['start_date']);
  }
  else {
    $startDate = new \DateTime();
    $startDate->setDate(date("Y"), 1, 1);
  }
  $form['valghalla_tfa_period_start_date']['#default_value'] = $startDate->format('Y-m-d 00:00:00');
  $startDateTs = $startDate->getTimestamp();
  $startDateHuman = $startDate->format("d/m/Y");

  // Getting end date.
  if (isset($_GET['end_date'])) {
    $endDate = date_create_from_format('d/m/Y', $_GET['end_date']);
  }
  else {
    $endDate = new \DateTime();
  }
  $form['valghalla_tfa_period_end_date']['#default_value'] = $endDate->format('Y-m-d 00:00:00');
  $endDateTs = $endDate->getTimestamp();
  $endDateHuman = $endDate->format("d/m/Y");

  $query = db_select('valghalla_tfa_sms_message', 'sms');
  $query->fields('sms');
  if ($startDateTs) {
    $query->condition('sms.created', $startDateTs, '>=');
  }
  if ($endDateTs) {
    $query->condition('sms.created', $endDateTs, '<=');
  }

  $count = $query->execute()->rowCount();

  $options = array(
    array(
      "Fra $startDateHuman til $endDateHuman",
      $count
    )
  );

  $form[] = array(
    '#markup' => theme('table', array('header' => $header, 'rows' => $options))
  );

  return $form;
}

/**
 * Valghalla TFA SMS Message report form custom submit.
 */
function valghalla_tfa_sms_message_report_form_custom_submit($form, &$form_state) {
  $values = $form_state['input'];

  $query = array();
  if (isset($values['valghalla_tfa_period_start_date']) && !empty($values['valghalla_tfa_period_start_date']['date'])) {
    $query['start_date'] = $values['valghalla_tfa_period_start_date']['date'];
  }
  if (isset($values['valghalla_tfa_period_end_date']) && !empty($values['valghalla_tfa_period_end_date']['date'])) {
    $query['end_date'] = $values['valghalla_tfa_period_end_date']['date'];
  }

  drupal_goto('admin/valghalla/valghalla_tfa/custom', array(
    'query' => $query
  ));
}


