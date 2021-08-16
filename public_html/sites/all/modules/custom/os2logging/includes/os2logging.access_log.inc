<?php

/**
 * Saved access log entry into a database.
 *
 * @param array $log_record
 *
 * @throws \Exception
 */
function os2logging_access_log_create($log_record) {
  $sid = $log_record['context']['sid'];
  $uid = $log_record['context']['uid'];
  $message = $log_record['message'];
  $ip = $log_record['extra']['ip'];
  $request_uri = $log_record['extra']['referrer'];
  $created = $log_record['context']['created'];

  // Do we have already this row in the databasse?
  $count = db_select('os2logging_access_log', 'o')
    ->fields('o')
    ->condition('sid', $sid)
    ->condition('uid', $uid)
    ->condition('ip', $ip)
    ->condition('created', $created)
    ->execute()
    ->rowCount();

  // If row is missing, Create the access log entry.
  if (!$count) {
    $id = db_insert('os2logging_access_log')
      ->fields([
        'sid' => $sid,
        'uid' => $uid,
        'message' => $message,
        'ip' => $ip,
        'request_uri' => $request_uri,
        'created' => $created,
      ])
      ->execute();

    return $id;
  }

  return 0;
}

/**
 * Clean up funciton.
 *
 * Deletes database logs that are older than the allowed period.
 */
function os2logging_access_log_cleanup() {
  $last_cleanup_run_ts = variable_get('os2logging_last_cleanup_run', 0);
  $now = time();

  $seconds_in_day = 24*60*60;

  // Proceed if the last cleanup happened more than a day ago.
  if (($now - $last_cleanup_run_ts) > $seconds_in_day) {
    $dblogs_store_period = variable_get('os2logging_dblogs_store_period', 180);

    // Delete entries that are older than a specified store period.
    $threshold_create_ts = $now - ($dblogs_store_period * $seconds_in_day);
    print_r($threshold_create_ts);

    db_delete('os2logging_access_log')
      ->condition('created', $threshold_create_ts, "<")
      ->execute();

    variable_set('os2logging_last_cleanup_run', time());
  }

}

