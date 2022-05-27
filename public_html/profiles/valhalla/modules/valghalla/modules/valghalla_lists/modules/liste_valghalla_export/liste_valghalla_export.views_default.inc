<?php

/**
 * @file
 * liste_valghalla_export.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function liste_valghalla_export_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'liste_valghalla_export';
  $view->description = 'lists of volunteers';
  $view->tag = 'valghalla, list';
  $view->base_table = 'node';
  $view->human_name = 'Liste Valghalla export';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Defaults */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['use_more_text'] = 'mere';
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'valghalla export volunteers list';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['distinct'] = TRUE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['submit_button'] = 'Udfør';
  $handler->display->display_options['exposed_form']['options']['reset_button_label'] = 'Gendan';
  $handler->display->display_options['exposed_form']['options']['exposed_sorts_label'] = 'Sortér efter';
  $handler->display->display_options['exposed_form']['options']['sort_asc_label'] = 'Stigende';
  $handler->display->display_options['exposed_form']['options']['sort_desc_label'] = 'Faldende';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '0';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'fields';
  /* Forbindelse: Felt: Parti (field_party) */
  $handler->display->display_options['relationships']['field_party_tid']['id'] = 'field_party_tid';
  $handler->display->display_options['relationships']['field_party_tid']['table'] = 'field_data_field_party';
  $handler->display->display_options['relationships']['field_party_tid']['field'] = 'field_party_tid';
  /* Forbindelse: Indhold: Valginfo (field_electioninfo) */
  $handler->display->display_options['relationships']['field_electioninfo_value']['id'] = 'field_electioninfo_value';
  $handler->display->display_options['relationships']['field_electioninfo_value']['table'] = 'field_data_field_electioninfo';
  $handler->display->display_options['relationships']['field_electioninfo_value']['field'] = 'field_electioninfo_value';
  $handler->display->display_options['relationships']['field_electioninfo_value']['delta'] = '-1';
  /* Felt: Indhold: Titel */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Navn';
  $handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
  /* Felt: Felt: Parti */
  $handler->display->display_options['fields']['entity_id_7']['id'] = 'entity_id_7';
  $handler->display->display_options['fields']['entity_id_7']['table'] = 'field_data_field_party';
  $handler->display->display_options['fields']['entity_id_7']['field'] = 'field_party';
  $handler->display->display_options['fields']['entity_id_7']['type'] = 'taxonomy_term_reference_plain';
  /* Felt: Indhold: CPR Nummer */
  $handler->display->display_options['fields']['entity_id']['id'] = 'entity_id';
  $handler->display->display_options['fields']['entity_id']['table'] = 'field_data_field_cpr_number';
  $handler->display->display_options['fields']['entity_id']['field'] = 'field_cpr_number';
  $handler->display->display_options['fields']['entity_id']['exclude'] = TRUE;
  /* Felt: Indhold: Email */
  $handler->display->display_options['fields']['entity_id_1']['id'] = 'entity_id_1';
  $handler->display->display_options['fields']['entity_id_1']['table'] = 'field_data_field_email';
  $handler->display->display_options['fields']['entity_id_1']['field'] = 'field_email';
  $handler->display->display_options['fields']['entity_id_1']['label'] = 'E-mail';
  $handler->display->display_options['fields']['entity_id_1']['exclude'] = TRUE;
  /* Felt: Indhold: Telefon */
  $handler->display->display_options['fields']['entity_id_2']['id'] = 'entity_id_2';
  $handler->display->display_options['fields']['entity_id_2']['table'] = 'field_data_field_phone';
  $handler->display->display_options['fields']['entity_id_2']['field'] = 'field_phone';
  $handler->display->display_options['fields']['entity_id_2']['exclude'] = TRUE;
  /* Felt: Felt: Valgsted */
  $handler->display->display_options['fields']['entity_id_3']['id'] = 'entity_id_3';
  $handler->display->display_options['fields']['entity_id_3']['table'] = 'field_data_field_polling_station';
  $handler->display->display_options['fields']['entity_id_3']['field'] = 'field_polling_station';
  $handler->display->display_options['fields']['entity_id_3']['label'] = 'valgsted';
  /* Felt: Field collection item: Status */
  $handler->display->display_options['fields']['entity_id_5']['id'] = 'entity_id_5';
  $handler->display->display_options['fields']['entity_id_5']['table'] = 'field_data_field_rsvp';
  $handler->display->display_options['fields']['entity_id_5']['field'] = 'field_rsvp';
  $handler->display->display_options['fields']['entity_id_5']['label'] = 'RSVP';
  /* Felt: Field collection item: Token */
  $handler->display->display_options['fields']['entity_id_6']['id'] = 'entity_id_6';
  $handler->display->display_options['fields']['entity_id_6']['table'] = 'field_data_field_token';
  $handler->display->display_options['fields']['entity_id_6']['field'] = 'field_token';
  $handler->display->display_options['fields']['entity_id_6']['label'] = 'Symbol';
  /* Felt: Indhold: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  /* Sorteringskriterie: Indhold: Titel */
  $handler->display->display_options['sorts']['title']['id'] = 'title';
  $handler->display->display_options['sorts']['title']['table'] = 'node';
  $handler->display->display_options['sorts']['title']['field'] = 'title';
  /* Kontekstuelt filter: Felt: Parti (field_party) */
  $handler->display->display_options['arguments']['field_party_tid']['id'] = 'field_party_tid';
  $handler->display->display_options['arguments']['field_party_tid']['table'] = 'field_data_field_party';
  $handler->display->display_options['arguments']['field_party_tid']['field'] = 'field_party_tid';
  $handler->display->display_options['arguments']['field_party_tid']['exception']['title'] = 'Alle';
  $handler->display->display_options['arguments']['field_party_tid']['default_argument_type'] = 'fixed';
  $handler->display->display_options['arguments']['field_party_tid']['summary']['format'] = 'default_summary';
  /* Kontekstuelt filter: Field collection item: Valg (field_election) */
  $handler->display->display_options['arguments']['field_election_target_id']['id'] = 'field_election_target_id';
  $handler->display->display_options['arguments']['field_election_target_id']['table'] = 'field_data_field_election';
  $handler->display->display_options['arguments']['field_election_target_id']['field'] = 'field_election_target_id';
  $handler->display->display_options['arguments']['field_election_target_id']['relationship'] = 'field_electioninfo_value';
  $handler->display->display_options['arguments']['field_election_target_id']['default_action'] = 'default';
  $handler->display->display_options['arguments']['field_election_target_id']['exception']['title'] = 'Alle';
  $handler->display->display_options['arguments']['field_election_target_id']['default_argument_type'] = 'php';
  $handler->display->display_options['arguments']['field_election_target_id']['default_argument_options']['code'] = 'return $_SESSION[\'election_nid\'];';
  $handler->display->display_options['arguments']['field_election_target_id']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_election_target_id']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_election_target_id']['summary_options']['items_per_page'] = '25';
  /* Filterkriterie: Indhold: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'volunteers' => 'volunteers',
  );
  /* Filterkriterie: Field collection item: Rolle (field_post_role) */
  $handler->display->display_options['filters']['field_post_role_target_id']['id'] = 'field_post_role_target_id';
  $handler->display->display_options['filters']['field_post_role_target_id']['table'] = 'field_data_field_post_role';
  $handler->display->display_options['filters']['field_post_role_target_id']['field'] = 'field_post_role_target_id';
  $handler->display->display_options['filters']['field_post_role_target_id']['relationship'] = 'field_electioninfo_value';
  $handler->display->display_options['filters']['field_post_role_target_id']['operator'] = '>=';
  $handler->display->display_options['filters']['field_post_role_target_id']['value']['value'] = '0';

  /* Display: Data Export CSV */
  $handler = $view->new_display('views_data_export', 'Data Export CSV', 'views_data_export_3');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '0';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'views_data_export_csv';
  $handler->display->display_options['style_options']['provide_file'] = 1;
  $handler->display->display_options['style_options']['parent_sort'] = 0;
  $handler->display->display_options['style_options']['quote'] = 1;
  $handler->display->display_options['style_options']['trim'] = 0;
  $handler->display->display_options['style_options']['replace_newlines'] = 0;
  $handler->display->display_options['style_options']['header'] = 1;
  $handler->display->display_options['style_options']['keep_html'] = 0;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Felt: Felt: Parti */
  $handler->display->display_options['fields']['entity_id_7']['id'] = 'entity_id_7';
  $handler->display->display_options['fields']['entity_id_7']['table'] = 'field_data_field_party';
  $handler->display->display_options['fields']['entity_id_7']['field'] = 'field_party';
  $handler->display->display_options['fields']['entity_id_7']['label'] = 'partibogstav';
  $handler->display->display_options['fields']['entity_id_7']['hide_alter_empty'] = FALSE;
  $handler->display->display_options['fields']['entity_id_7']['type'] = 'taxonomy_term_reference_plain';
  /* Felt: Indhold: CPR Nummer */
  $handler->display->display_options['fields']['entity_id']['id'] = 'entity_id';
  $handler->display->display_options['fields']['entity_id']['table'] = 'field_data_field_cpr_number';
  $handler->display->display_options['fields']['entity_id']['field'] = 'field_cpr_number';
  $handler->display->display_options['fields']['entity_id']['label'] = 'cprnummer';
  $handler->display->display_options['fields']['entity_id']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['entity_id']['hide_alter_empty'] = FALSE;
  /* Felt: Indhold: Titel */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'fulde navn';
  $handler->display->display_options['fields']['title']['hide_alter_empty'] = FALSE;
  $handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
  /* Felt: Indhold: Email */
  $handler->display->display_options['fields']['entity_id_1']['id'] = 'entity_id_1';
  $handler->display->display_options['fields']['entity_id_1']['table'] = 'field_data_field_email';
  $handler->display->display_options['fields']['entity_id_1']['field'] = 'field_email';
  $handler->display->display_options['fields']['entity_id_1']['label'] = 'email adresse';
  $handler->display->display_options['fields']['entity_id_1']['hide_alter_empty'] = FALSE;
  $handler->display->display_options['fields']['entity_id_1']['type'] = 'email_plain';
  /* Felt: Indhold: Telefon */
  $handler->display->display_options['fields']['entity_id_2']['id'] = 'entity_id_2';
  $handler->display->display_options['fields']['entity_id_2']['table'] = 'field_data_field_phone';
  $handler->display->display_options['fields']['entity_id_2']['field'] = 'field_phone';
  $handler->display->display_options['fields']['entity_id_2']['label'] = 'telefonnummer';
  $handler->display->display_options['fields']['entity_id_2']['hide_alter_empty'] = FALSE;
  $handler->display->display_options['fields']['entity_id_2']['type'] = 'text_plain';
  /* Felt: Field collection item: Valgsted */
  $handler->display->display_options['fields']['field_vlnt_station']['id'] = 'field_vlnt_station';
  $handler->display->display_options['fields']['field_vlnt_station']['table'] = 'field_data_field_vlnt_station';
  $handler->display->display_options['fields']['field_vlnt_station']['field'] = 'field_vlnt_station';
  $handler->display->display_options['fields']['field_vlnt_station']['relationship'] = 'field_electioninfo_value';
  $handler->display->display_options['fields']['field_vlnt_station']['label'] = 'valgsted';
  $handler->display->display_options['fields']['field_vlnt_station']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_vlnt_station']['settings'] = array(
    'link' => 0,
  );
  /* Felt: Field collection item: Status */
  $handler->display->display_options['fields']['field_rsvp']['id'] = 'field_rsvp';
  $handler->display->display_options['fields']['field_rsvp']['table'] = 'field_data_field_rsvp';
  $handler->display->display_options['fields']['field_rsvp']['field'] = 'field_rsvp';
  $handler->display->display_options['fields']['field_rsvp']['relationship'] = 'field_electioninfo_value';
  /* Felt: Field collection item: Rolle */
  $handler->display->display_options['fields']['field_post_role']['id'] = 'field_post_role';
  $handler->display->display_options['fields']['field_post_role']['table'] = 'field_data_field_post_role';
  $handler->display->display_options['fields']['field_post_role']['field'] = 'field_post_role';
  $handler->display->display_options['fields']['field_post_role']['relationship'] = 'field_electioninfo_value';
  $handler->display->display_options['fields']['field_post_role']['settings'] = array(
    'link' => 0,
  );
  $handler->display->display_options['path'] = 'valghalla_lists/export_csv';
  $handler->display->display_options['displays'] = array(
    'page_1' => 'page_1',
    'default' => 0,
  );
  $handler->display->display_options['use_batch'] = 'batch';
  $handler->display->display_options['segment_size'] = '100';

  /* Display: Data Export XLS */
  $handler = $view->new_display('views_data_export', 'Data Export XLS', 'views_data_export_1');
  $handler->display->display_options['defaults']['hide_admin_links'] = FALSE;
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '0';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'views_data_export_xls';
  $handler->display->display_options['style_options']['provide_file'] = 1;
  $handler->display->display_options['style_options']['parent_sort'] = 0;
  $handler->display->display_options['defaults']['fields'] = FALSE;
  /* Felt: Felt: Parti */
  $handler->display->display_options['fields']['entity_id_7']['id'] = 'entity_id_7';
  $handler->display->display_options['fields']['entity_id_7']['table'] = 'field_data_field_party';
  $handler->display->display_options['fields']['entity_id_7']['field'] = 'field_party';
  $handler->display->display_options['fields']['entity_id_7']['label'] = 'partibogstav';
  $handler->display->display_options['fields']['entity_id_7']['hide_alter_empty'] = FALSE;
  $handler->display->display_options['fields']['entity_id_7']['type'] = 'taxonomy_term_reference_plain';
  /* Felt: Indhold: CPR Nummer */
  $handler->display->display_options['fields']['entity_id']['id'] = 'entity_id';
  $handler->display->display_options['fields']['entity_id']['table'] = 'field_data_field_cpr_number';
  $handler->display->display_options['fields']['entity_id']['field'] = 'field_cpr_number';
  $handler->display->display_options['fields']['entity_id']['label'] = 'cprnummer';
  $handler->display->display_options['fields']['entity_id']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['entity_id']['hide_alter_empty'] = FALSE;
  /* Felt: Indhold: Titel */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'fulde navn';
  $handler->display->display_options['fields']['title']['hide_alter_empty'] = FALSE;
  $handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
  /* Felt: Indhold: Email */
  $handler->display->display_options['fields']['entity_id_1']['id'] = 'entity_id_1';
  $handler->display->display_options['fields']['entity_id_1']['table'] = 'field_data_field_email';
  $handler->display->display_options['fields']['entity_id_1']['field'] = 'field_email';
  $handler->display->display_options['fields']['entity_id_1']['label'] = 'email adresse';
  $handler->display->display_options['fields']['entity_id_1']['hide_alter_empty'] = FALSE;
  $handler->display->display_options['fields']['entity_id_1']['type'] = 'email_plain';
  /* Felt: Indhold: Telefon */
  $handler->display->display_options['fields']['entity_id_2']['id'] = 'entity_id_2';
  $handler->display->display_options['fields']['entity_id_2']['table'] = 'field_data_field_phone';
  $handler->display->display_options['fields']['entity_id_2']['field'] = 'field_phone';
  $handler->display->display_options['fields']['entity_id_2']['label'] = 'telefonnummer';
  $handler->display->display_options['fields']['entity_id_2']['hide_alter_empty'] = FALSE;
  $handler->display->display_options['fields']['entity_id_2']['type'] = 'text_plain';
  /* Felt: Field collection item: Valgsted */
  $handler->display->display_options['fields']['field_vlnt_station']['id'] = 'field_vlnt_station';
  $handler->display->display_options['fields']['field_vlnt_station']['table'] = 'field_data_field_vlnt_station';
  $handler->display->display_options['fields']['field_vlnt_station']['field'] = 'field_vlnt_station';
  $handler->display->display_options['fields']['field_vlnt_station']['relationship'] = 'field_electioninfo_value';
  $handler->display->display_options['fields']['field_vlnt_station']['label'] = 'valgsted';
  $handler->display->display_options['fields']['field_vlnt_station']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_vlnt_station']['settings'] = array(
    'link' => 0,
  );
  /* Felt: Field collection item: Status */
  $handler->display->display_options['fields']['field_rsvp']['id'] = 'field_rsvp';
  $handler->display->display_options['fields']['field_rsvp']['table'] = 'field_data_field_rsvp';
  $handler->display->display_options['fields']['field_rsvp']['field'] = 'field_rsvp';
  $handler->display->display_options['fields']['field_rsvp']['relationship'] = 'field_electioninfo_value';
  /* Felt: Field collection item: Rolle */
  $handler->display->display_options['fields']['field_post_role']['id'] = 'field_post_role';
  $handler->display->display_options['fields']['field_post_role']['table'] = 'field_data_field_post_role';
  $handler->display->display_options['fields']['field_post_role']['field'] = 'field_post_role';
  $handler->display->display_options['fields']['field_post_role']['relationship'] = 'field_electioninfo_value';
  $handler->display->display_options['fields']['field_post_role']['settings'] = array(
    'link' => 0,
  );
  $handler->display->display_options['path'] = 'valghalla_lists/export_xls';
  $handler->display->display_options['displays'] = array(
    'page_1' => 'page_1',
    'default' => 0,
  );
  $handler->display->display_options['use_batch'] = 'batch';
  $handler->display->display_options['segment_size'] = '100';
  $translatables['liste_valghalla_export'] = array(
    t('Defaults'),
    t('mere'),
    t('Udfør'),
    t('Gendan'),
    t('Sortér efter'),
    t('Stigende'),
    t('Faldende'),
    t('term fra field_party'),
    t('field collection item from field_electioninfo'),
    t('Navn'),
    t('Parti'),
    t('CPR Nummer'),
    t('E-mail'),
    t('Telefon'),
    t('valgsted'),
    t('RSVP'),
    t('Symbol'),
    t('Nid'),
    t('Alle'),
    t('Data Export CSV'),
    t('partibogstav'),
    t('cprnummer'),
    t('fulde navn'),
    t('email adresse'),
    t('telefonnummer'),
    t('Status'),
    t('Rolle'),
    t('Data Export XLS'),
  );
  $export['liste_valghalla_export'] = $view;

  return $export;
}
