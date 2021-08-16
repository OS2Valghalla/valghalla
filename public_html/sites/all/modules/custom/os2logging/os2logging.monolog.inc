<?php

/**
 * @file
 * os2logging.monolog.inc
 */

/**
 * Implements hook_default_monolog_profiles().
 */
function os2logging_default_monolog_profiles() {
  $export = array();

  $profile = new stdClass();
  $profile->disabled = FALSE; /* Edit this to true to make a default profile disabled initially */
  $profile->api_version = 1;
  $profile->name = 'os2logging';
  $profile->options = array(
    'handlers' => array(
      'os2logging_json_file_handler' => array(
        'weight' => '-9',
        'label' => 'OS2Logging JSON Rotation file handler',
        'level' => '200',
        'bubble' => 1,
        'filepath' => 'private://log/os2logging_node_access.log',
        'max_files' => '180',
        'handler' => 'os2logging_json_rotation_file',
      ),
      'os2logging_accesslog_entity_hand' => array(
        'weight' => '-8',
        'label' => 'OS2Logging AccessLog entity handler',
        'level' => '200',
        'bubble' => '1',
        'handler' => 'os2logging_accesslog',
      ),
    ),
    'label' => 'OS2Logging',
  );
  $export['os2logging'] = $profile;

  $profile = new stdClass();
  $profile->disabled = FALSE; /* Edit this to true to make a default profile disabled initially */
  $profile->api_version = 1;
  $profile->name = 'watchdog';
  $profile->options = array(
    'handlers' => array(
      'rotating_file_handler' => array(
        'weight' => 1,
        'label' => 'Rotating File Handler',
        'level' => '200',
        'bubble' => 1,
        'filepath' => 'private://log/watchdog.log',
        'max_files' => '60',
        'handler' => 'rotating_file',
      ),
    ),
    'label' => 'Watchdog',
  );
  $export['watchdog'] = $profile;

  return $export;
}
