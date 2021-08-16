<?php

/**
 * @file
 * valghalla_simplify.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function valghalla_simplify_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}
