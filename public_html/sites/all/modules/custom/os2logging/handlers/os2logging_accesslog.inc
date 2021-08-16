<?php

/**
 * @file
 * Handler include for AccessLogHandler.
 */

/**
 * Monolog loader callback; Loads a AccessLogHandler handler.
 */
function os2logging_accesslog_handler_loader($options) {
  return new AccessLogHandler($options['level']);
}

/**
 * Monolog settings form; Settings for the AccessLogHandler handler.
 */
function os2logging_accesslog_handler_settings(&$form, &$form_state, $profile, array $handler) {
  $form['bubble']['#access'] = FALSE;
}
