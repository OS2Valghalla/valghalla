<?php

use Drupal\Component\Serialization\Json;
use Drupal\os2web_logging\Entity\AccessLog;

module_load_include('inc', 'os2logging', 'includes/os2logging.access_log');

/**
 * Settings form.
 */
function os2logging_settings_form($form, $form_state) {
  $form[] = array(
    '#markup' => t('Logs messages can be seen on the <a href="@reports_page">reports page</a>.', array('@reports_page' => url('admin/reports/os2logging'))),
  );

  // Selecting node types.
  $form[] = array(
    '#markup' => t('Choose the node types you wish to be monitored.'),
    '#prefix' => '<p>',
    '#suffix' => '<hr /></p>',
  );

  $node_types = node_type_get_types();
  foreach ((array) $node_types as $nd) {
    $form['os2logging-type'][$nd->type] = array(
      '#type' => 'checkbox',
      '#title' => $nd->name,
      '#default_value' => _os2logging_node_type_chosen($nd->type),
    );
  }

  // Anonymous logging.
  $form[] = array(
    '#markup' => t('Anonymous logging'),
    '#prefix' => '<p>',
    '#suffix' => '<hr /></p>',
  );
  $form['os2logging_log_anonymous_user'] = [
    '#type' => 'checkbox',
    '#title' => t('Log anonymous user actions'),
    '#description' => t('If anonymous user actions are being logged'),
    '#default_value' => variable_get('os2logging_log_anonymous_user'),
  ];

  // Database logging settings.
  $form['os2logging_dblogs_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Database logs'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['os2logging_dblogs_fs']['os2logging_dblogs_store_period'] = array(
    '#type' => 'textfield',
    '#title' => t('Store database logs for this period'),
    '#field_suffix' => t('days'),
    '#size' => 5,
    '#attributes' => array(
      ' type' => 'number',
    ),
    '#description' => t('Database logs will be stored for the selected number of days, after that they will be automatically deleted (cleanup is done daily)'),
    '#default_value' => variable_get('os2logging_dblogs_store_period', 180),
  );

  // File logging handler settings.
  $rotating_file_handler = monolog_handler_load('os2logging_json_file_handler', 'os2logging');

  $form['os2logging_file_logs_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('File logs'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['os2logging_file_logs_fs']['os2logging_file_logs_store_period'] = array(
    '#type' => 'textfield',
    '#title' => t('Store log files for this period'),
    '#field_suffix' => t('days'),
    '#size' => 5,
    '#attributes' => array(
      ' type' => 'number',
    ),
    '#description' => t('Log file will be stored for the selected number of days, after that they will be automatically deleted'),
    '#default_value' => (int) $rotating_file_handler['max_files'],
  );
  $form['os2logging_file_logs_fs']['os2logging_file_logs_filepath'] = array(
    '#type' => 'textfield',
    '#title' => t('Store log filepath'),
    '#description' => t('Logs will be saved in this path'),
    '#default_value' =>  $rotating_file_handler['filepath'],
  );

  $form['logs_import_file_detail'] = [
    '#type' => 'fieldset',
    '#title' => t('Logs import'),
  ];

  $fileImportOptions = array();
  if ($rotating_file_handler['filepath']) {
    $pathinfo = pathinfo($rotating_file_handler['filepath']);

    $logsDirectory = $pathinfo['dirname'];
    $logFilename = $pathinfo['filename'];
    $storedLogFiles = file_scan_directory($logsDirectory, "/$logFilename-\d{4}-\d{2}-\d{2}\.(log|gz)/");

    foreach ($storedLogFiles as $file) {
      $fileImportOptions[$file->uri] = $file->filename;
    }
    arsort($fileImportOptions);
  }

  $form['logs_import_file_detail']['logs_import_files_select'] = [
    '#type' => 'checkboxes',
    '#options' => $fileImportOptions,
    '#title' => t('Import from existing files'),
    '#description' => t('Archived log files will be automatically extracted'),
  ];

  $form['logs_import_file_detail']['logs_import_file'] = [
    '#type' => 'file',
    '#title' => t('Import logs from uploaded file'),
    '#description' => t('Logs will be imported form the uploaded file (.log or .json)'),
  ];

  $form['#submit'][] = 'os2logging_settings_form_submit';

  return system_settings_form($form);
}

/**
 * Settings form submit validate.
 */
function os2logging_settings_form_validate($form, &$form_state) {
  if (!file_prepare_directory(dirname($form_state['values']['os2logging_file_logs_filepath']))) {
    form_set_error('os2logging_file_logs_filepath', t('Directory cannot be created or is not writable'));
  }
}

/**
 * Settings form submit handler.
 */
function os2logging_settings_form_submit($form, &$form_state) {
  // Storing files to import.
  $importFileUris = array();

  // Checking file if we have an uploaded file to import from.
  $uploadedFile = os2logging_settings_upload_import_file();

  if ($uploadedFile) {
    drupal_set_message(t('File uploaded successfully: @file', ['@file' => $uploadedFile->uri]));
    $importFileUris[] = $uploadedFile->uri;
  }

  // Checking if we have any files selected to import from.
  if ($selectedFiles = $form_state['values']['logs_import_files_select']) {
    foreach ($selectedFiles as $uri) {
      if ($uri) {
        $importFileUris[] = $uri;
      }
    }
    unset($form_state['values']['logs_import_files_select']);
  }

  // Import logs from the files.
  if (!empty($importFileUris)) {
    $importedLogs = os2logging_import_logs_from_uris($importFileUris);
    if ($importedLogs) {
      // To prevent old logs from being deleted on next cron (might happen at
      // any time), update the last os2logging_last_cleanup_run.
      //
      // This will make logs stored in the database for the next 24h.
      variable_set('os2logging_last_cleanup_run', time());
    }
  }

  $node_types = node_type_get_types();
  $chosen_node_types = array();

  foreach ($node_types as $n_type) {
    if ( $form_state['values'][$n_type->type] == '1' )
    {
      $chosen_node_types[] = $n_type->type;
    }
  }
  variable_set('os2logging_chosen_node_types', $chosen_node_types);

  // Updating monolog profile.
  $monolog_profile = monolog_profile_load('os2logging');
  $monolog_profile->options['handlers']['os2logging_json_file_handler']['max_files'] = $form_state['values']['os2logging_file_logs_store_period'];
  $monolog_profile->options['handlers']['os2logging_json_file_handler']['filepath'] = $form_state['values']['os2logging_file_logs_filepath'];

  monolog_profile_save($monolog_profile);
}

/**
 * Helper function to get the list of chosen node types.
 *
 * @return bool|array
 */
function _os2logging_node_type_chosen($node_type) {
  $types_list = variable_get('os2logging_chosen_node_types');
  return (is_array($types_list) && in_array($node_type, $types_list));
}

/**
 * Uploads file from field logs_import_file.
 */
function os2logging_settings_upload_import_file() {
  // Add validator for your file type etc.
  $validators = array('file_validate_extensions' => array('log', 'json'));

  $file = file_save_upload('logs_import_file', $validators, FALSE, 0);

  return $file;
}

/**
 * OS2Logging access logs table form.
 */
function os2logging_access_logs_table_form($form, &$form_state) {
  $header = array(
    'created' => array(
      'data' => t('Created'),
      'type' => 'property',
      'field' => 'a.created',
      'sort' => 'desc'
    ),
    'node_title' => array(
      'data' => t('Node title'),
      'type' => 'property',
      'field' => 'n.title',
    ),
    'message' => array(
      'data' => t('Message'),
      'type' => 'property',
      'field' => 'a.message',
    ),
    'user' => array(
      'data' => t('User'),
      'type' => 'property',
      'field' => 'u.name',
    ),
    'user_ip' => array(
      'data' => t('User\'s IP'),
      'type' => 'property',
      'field' => 'a.ip',
    ),
    'request_uri' => array(
      'data' => t('Request URI'),
      'type' => 'property',
      'field' => 'a.request_uri',
    ),
  );

  $query = db_select('os2logging_access_log', 'a');
  $query->join('node', 'n', 'n.nid=a.sid');
  $query->join('users', 'u', 'u.uid=a.uid');
  $query->fields('a');
  $query->fields('n', array('title'));
  $query->fields('u', array('name'));

  $query = $query->extend('PagerDefault')
    ->extend('TableSort')
    ->limit(50)
    ->orderByHeader($header);

  $search_params = array();
  if (!empty($_GET['free_text_filter'])) {
    $search_params['free_text_filter'] = $_GET['free_text_filter'];

    $or = db_or();
    $or->condition('a.message', '%' . $search_params['free_text_filter'] . '%', 'LIKE');
    $or->condition('n.title', '%' . $search_params['free_text_filter'] . '%', 'LIKE');
    $or->condition('u.name', '%' . $search_params['free_text_filter'] . '%', 'LIKE');
    $query->condition($or);
  }
  if (!empty($_GET['from_date_filter'])) {
    $search_params['from_date_filter'] = implode(' ', $_GET['from_date_filter']);
    $query->condition('a.created', strtotime($search_params['from_date_filter']), '>=');
  }
  if (!empty($_GET['to_date_filter'])) {
    $search_params['to_date_filter'] = implode(' ', $_GET['to_date_filter']);
    // Adding 59 seconds, so that the entire minute is included.
    $search_params['to_date_filter'] .= ':59';

    $query->condition('a.created', strtotime($search_params['to_date_filter']), '<=');
  }
  if (!empty($_GET['ip_filter'])) {
    $search_params['ip_filter'] = $_GET['ip_filter'];
    $query->condition('a.ip', $search_params['ip_filter'] . '%', 'LIKE');
  }

  $result = $query->execute()->fetchAll();

  $options = array();
  foreach ($result as $entry) {
    $options[] = array(
      'created' => format_date($entry->created, 'short'),
      'node_title' => l($entry->title, 'node/' . $entry->sid),
      'message' => $entry->message,
      'user' => ($entry->uid) ? l($entry->name, 'user/' . $entry->uid) : t('Anonym'),
      'user_ip' => $entry->ip,
      'request_uri' => $entry->request_uri,
    );
  }

  /** Filter START */
  $form['search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic Search'),
    '#collapsible' => TRUE,
    '#collapsed' => empty($search_params),
  );

  $form['search']['free_text_filter'] = array(
    '#type' => 'textfield',
    '#title' => t('Free text'),
    '#default_value' => isset($search_params['free_text_filter']) ? $search_params['free_text_filter'] : '',
  );

  $form['search']['from_date_filter'] = array(
    '#type' => 'date_popup',
    '#title' => t('From date filter'),
    '#default_value' => isset($search_params['from_date_filter']) ? date('Y-m-d H:i:s', strtotime($search_params['from_date_filter'])) : '',
  );

  $form['search']['to_date_filter'] = array(
    '#type' => 'date_popup',
    '#title' => t('To date filter'),
    '#default_value' => isset($search_params['to_date_filter']) ? date('Y-m-d H:i:s', strtotime($search_params['to_date_filter'])) : '',
  );

  $form['search']['ip_filter'] = array(
    '#type' => 'textfield',
    '#title' => t('IP'),
    '#default_value' => isset($search_params['ip_filter']) ? $search_params['ip_filter'] : '',
    '#description' => t('supports partial search'),
    '#attributes' => array(
      'placeholder' => '192.168'
    )
  );

  $form['search']['search_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );
  $form['search']['search_reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
  );
  /** Filter END */

  $form['entities'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#attributes' => array('class' => array('entity-sort-table')),
    '#empty' => t('No content.'),
  );

  $form['pager'] = array('#theme' => 'pager');

  return $form;
}

/**
 * OS2Logging access logs table form submit.
 */
function os2logging_access_logs_table_form_submit($form, &$form_state) {
  $query = array();

  // If submitted via search button - populate the search queury.
  if ($form_state['triggering_element']['#id'] == 'edit-search-submit') {
    $values = $form_state['input'];

    $query = array();
    if (!empty($values['free_text_filter'])) {
      $query['free_text_filter'] = $values['free_text_filter'];
    }
    if (isset($values['from_date_filter']) && !empty($values['from_date_filter']['date'])) {
      $query['from_date_filter'] = $values['from_date_filter'];
    }
    if (isset($values['to_date_filter']) && !empty($values['to_date_filter']['date'])) {
      $query['to_date_filter'] = $values['to_date_filter'];
    }
    if (isset($values['xml_id_filter'])) {
      $query['xml_id_filter'] = $values['xml_id_filter'];
    }
    if (isset($values['ip_filter'])) {
      $query['ip_filter'] = $values['ip_filter'];
    }
  }

  drupal_goto('admin/reports/os2logging', array(
    'query' => $query
  ));
}

/**
 * Imports logs from file.
 *
 * Reads the files from provided URIs and creates log entries.
 *
 * @param array $uris
 *   Array of files uris.
 *
 * @return int
 *   Number of imported logs entities.
 */
function os2logging_import_logs_from_uris(array $uris) {
  $total_imported = 0;

  foreach ($uris as $uri) {
    $lines_counter = 0;
    $imported_items_counter = 0;

    $handle = gzopen($uri, "r");
    if ($handle) {
      while (($line = gzgets($handle)) !== FALSE) {
        $lines_counter++;
        $record = json_decode($line, TRUE);

        // Discarding messages that do not follow format.
        if (empty($record) ||
          !array_key_exists('context', $record) ||
          !array_key_exists('extra', $record) ||
          !array_key_exists('message', $record) ||
          !array_key_exists('datetime', $record)) {
          continue;
        }

        // Create new log entry.
        if (os2logging_access_log_create($record)) {
          $imported_items_counter++;
          $total_imported++;
        }
      }
      fclose($handle);

      drupal_set_message(t('Log messages @imported out of @total were imported', array(
        '@imported' => $imported_items_counter,
        '@total' => $lines_counter,
      )));
    }
    else {
      drupal_set_message(t('Could not import from file @file', array('@file' => $uri)), 'warning');
    }
  }

  return $total_imported;
}
