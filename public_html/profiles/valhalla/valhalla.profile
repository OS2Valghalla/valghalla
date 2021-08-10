<?php

/**
 * @file
 * This file includes all hooks to proper set up profile during install
 */

/**
 * Name of profile; visible in profile selection form.
 */
define('PROFILE_NAME', 'Valghalla');

/**
 * Description of profile; visible in profile selection form.
 */
define('PROFILE_DESCRIPTION', 'Generisk Installation Valghalla.');

/**
 * Implements hook_form_FORM_ID_alter().
 */
function valhalla_form_install_configure_form_alter(&$form, $form_state) {
  // Pre-populate the site name with the server name.
  $form['site_information']['site_name']['#default_value'] = $_SERVER['SERVER_NAME'];

  $form['#submit'][] = 'valhalla_settings_extra_submit';
}

/**
 * Implements hook_install_tasks().
 */
function valhalla_install_tasks(&$install_state) {
  $tasks = array(
    'valhalla_enable_modules' => array(
      'display_name' => st('Enable additional modules'),
      'display' => FALSE,
      'type' => 'normal',
    ),
    'valhalla_theme_settings' => array(
      'display_name' => st('Theme settings'),
      'display' => FALSE,
      'type' => 'normal',
    ),
    'valhalla_apply_updates' => array(
      'display_name' => st('Apply updates'),
      'display' => FALSE,
      'type' => 'normal',
    ),
  );

  return $tasks;
}

/**
 * Extra submit handler.
 */
function valhalla_settings_extra_submit(&$form, $form_state) {
  valhalla_theme_settings();
}

/**
 * Enable additional modules.
 */
function valhalla_enable_modules() {
  $modules = array(
    // Contrib modules.
    'ckeditor',
    'field_ui',
    'mailsystem',
    'mimemail',
    'views_ui',
    'contextual',
    'shortcut',
    // Valghalla modules.
    'valghalla_lists',
    'liste_beskeder',
    'liste_frivillige_uden_email',
    'liste_m_cpr_nummer',
    'liste_parti_oversigt',
    'liste_valghalla_export',
    'liste_valghalla_kvittering',
    'valghalla_notifications',
    'valghalla_eboks',
    'valghalla_mail',
    'valghalla_sms',
    'valghalla_volunteer_validator',
    'valghalla_volunteers_import',
    'valghalla_volunteers_invite',
    'vcv_serviceplatformen',
    'vcv_person_lookup_extended',
    'vvv_validate_age',
    'valghalla_status_report',
  );
  module_enable($modules);
}

/**
 * Set theme settings.
 */
function valhalla_theme_settings() {
  $variables["theme_default"] = 'site';

  $variables["theme_site_settings"] = array(
    'toggle_logo' => 1,
    'toggle_name' => 0,
    'toggle_slogan' => 0,
    'toggle_node_user_picture' => 1,
    'toggle_comment_user_picture' => 1,
    'toggle_comment_user_verification' => 0,
    'toggle_favicon' => 1,
    'toggle_main_menu' => 1,
    'toggle_secondary_menu' => 1,
    'default_logo' => 1,
    'default_favicon' => 1,
    'favicon_path' => '',
    'favicon_upload' => '',
    'general__active_tab' => 'edit-logo',
    'bootstrap__active_tab' => 'edit-components',
    'bootstrap_breadcrumb' => '1',
    'bootstrap_breadcrumb_home' => 0,
    'bootstrap_breadcrumb_title' => 1,
    'bootstrap_navbar_position' => 'static-top',
    'bootstrap_navbar_inverse' => 0,
    'bootstrap_region_well-navigation' => '',
    'bootstrap_region_well-header' => '',
    'bootstrap_region_well-highlighted' => '',
    'bootstrap_region_well-help' => '',
    'bootstrap_region_well-content' => '',
    'bootstrap_region_well-sidebar_first' => 'well',
    'bootstrap_region_well-sidebar_second' => '',
    'bootstrap_region_well-footer' => '',
    'bootstrap_region_well-page_top' => '',
    'bootstrap_region_well-page_bottom' => '',
    'bootstrap_region_well-dashboard_main' => '',
    'bootstrap_region_well-dashboard_sidebar' => '',
    'bootstrap_region_well-dashboard_inactive' => '',
    'bootstrap_anchors_fix' => 1,
    'bootstrap_anchors_smooth_scrolling' => 1,
    'bootstrap_popover_enabled' => 1,
    'bootstrap_popover_animation' => 1,
    'bootstrap_popover_html' => 0,
    'bootstrap_popover_placement' => 'right',
    'bootstrap_popover_selector' => '',
    'bootstrap_popover_trigger' => array(
      'click' => 'click',
      'hover' => 0,
      'focus' => 0,
      'manual' => 0,
    ),
    'bootstrap_popover_title' => '',
    'bootstrap_popover_content' => '',
    'bootstrap_popover_delay' => '0',
    'bootstrap_popover_container' => 'body',
    'bootstrap_tooltip_enabled' => 1,
    'bootstrap_tooltip_descriptions' => 1,
    'bootstrap_tooltip_animation' => 1,
    'bootstrap_tooltip_html' => 0,
    'bootstrap_tooltip_placement' => 'auto left',
    'bootstrap_tooltip_selector' => '',
    'bootstrap_tooltip_trigger' =>
    array(
      'hover' => 'hover',
      'focus' => 'focus',
      'click' => 0,
      'manual' => 0,
    ),
    'bootstrap_tooltip_delay' => '0',
    'bootstrap_tooltip_container' => 'body',
    'bootstrap_cdn' => '3.0.2',
    'bootstrap_bootswatch' => '',
    'bootstrap_rebuild_registry' => 0,
    'bootstrap_toggle_jquery_error' => 0,
  );

  $variables["preprocess_css"] = 1;

  // Don't use admin theme, when editing content.
  $variables["node_admin_theme"] = '0';

  // JQuery update.
  $variables["jquery_update_compression_type"] = 'min';
  $variables["jquery_update_jquery_admin_version"] = '';
  $variables["jquery_update_jquery_cdn"] = 'none';
  $variables["jquery_update_jquery_version"] = '1.8';

  // Simplify.
  $variables["simplify_blocks_global"] = array();
  $variables["simplify_comments_election"] = array();
  $variables["simplify_comments_global"] = array();
  $variables["simplify_nodes_election"] = array();
  $variables["simplify_nodes_global"] = array(
    0 => 'author',
    1 => 'format',
    2 => 'options',
    3 => 'revision',
    4 => 'comment',
    5 => 'menu',
    6 => 'path',
  );

  $variables["simplify_taxonomy_global"] = array(
    0 => 'format',
    1 => 'relations',
    2 => 'path',
  );

  $variables["simplify_users_global"] = array();

  $variables["valghalla_limit_uris"] = TRUE;

  $variables["jquery_update_jquery_version"] = '1.10';
  $variables["jquery_update_jquery_cdn"] = 'none';
  $variables["jquery_update_compression_type"] = '';
  $variables["jquery_update_jquery_admin_version"] = '';

  foreach ($variables as $variable => $value) {
    variable_set($variable, $value);
  }

  // Set default home page.
  variable_set('site_frontpage', 'status');
}

/**
 * Apply valghalla update during installtion.
 */
function valhalla_apply_updates() {
  module_load_include('install', 'valghalla', 'valghalla');
  valghalla_update_7106();
  valghalla_update_7107();
  valghalla_update_7109();
  valghalla_update_7114();
}
