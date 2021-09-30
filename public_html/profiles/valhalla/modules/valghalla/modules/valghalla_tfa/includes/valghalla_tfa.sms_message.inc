<?php

/**
* Saved access log entry into a database.
*
* @param array $log_record
*
* @throws \Exception
*/
function valghalla_tfa_sms_message_create($smsReceiver) {
  $uid = 0;
  $phone = '';

  // Trying to populate the context.
  if ($smsReceiver) {
    $uid = $smsReceiver->uid;
    $phone = $smsReceiver->field_mobile_phone['und'][0]['value'];
  }

  $requestUri = request_uri();
  if (strlen($requestUri) > 500) {
    $requestUri = substr($requestUri, 0, 499);
  }

  $id = db_insert('valghalla_tfa_sms_message')
    ->fields([
      'uid' => $uid,
      'phone' => $phone,
      'ip' => ip_address(),
      'request_uri' => $requestUri,
      'created' => REQUEST_TIME,
    ])
    ->execute();

  return $id;
}
