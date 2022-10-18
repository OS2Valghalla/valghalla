<?php

/**
 * Form callback for "Deltagere duplikater".
 */
function liste_deltager_duplikater_form($form, &$form_state) {
  $election_nid = NULL;
  if (isset($form_state['storage']['election_nid'])) {
    $election_nid = $form_state['storage']['election_nid'];
  }
  elseif (!empty($_SESSION['election_nid'])) {
    $election_nid = $_SESSION['election_nid'];
  }

  valghalla_liste_deltager_form_add_election_filter($form, $election_nid);

  $cpr_duplicates_query = db_select('field_data_field_cpr_number', 'f')
    ->fields('f', array('field_cpr_number_value'));

  $cpr_duplicates_query->addExpression('count(f.field_cpr_number_value)', 'count');
  $cpr_duplicates_query->groupBy('f.field_cpr_number_value');
  $cpr_duplicates_query->having('count > 1');
  $cpr_duplicates = $cpr_duplicates_query->execute()->fetchCol();

  $rows = [];

  if (!empty($cpr_duplicates)) {
    $query = db_select('node', 'n')
      ->fields('n', array('nid'))
      ->orderBy('title')
      ->condition('type', 'volunteers');

    $query->leftJoin('field_data_field_cpr_number', 'cpr', 'n.nid = cpr.entity_id');
    $query->condition('cpr.field_cpr_number_value', $cpr_duplicates, 'IN');

    if (!empty($election_nid)) {
      $inner = new EntityFieldQuery();
      $fc_info = $inner->entityCondition('entity_type', 'field_collection_item')
        ->entityCondition('bundle', 'field_electioninfo')
        ->fieldCondition('field_election', 'target_id', $election_nid)
        ->propertyCondition('archived', 0)
        ->execute();

      if (!empty($fc_info['field_collection_item'])) {
        $keys = array_keys($fc_info['field_collection_item']);
        $query->leftJoin('field_data_field_electioninfo', 'fci', 'n.nid = fci.entity_id');
        $query->condition('fci.field_electioninfo_value', $keys, 'IN');
      }
    }

    $result = $query->execute()->fetchAll();
    foreach ($result as $row) {
      $volunteer = node_load($row->nid);
      $wrapper = entity_metadata_wrapper('node', $volunteer);
      $rows[$volunteer->nid] = array(
        $wrapper->title->value(),
        $wrapper->field_cpr_number->value(),
        ($wrapper->field_party->value()) ? $wrapper->field_party->value()->name : '',
        l(t('Edit'), 'node/' . $volunteer->nid . '/edit', array('query' => array('destination' => current_path()))),
      );
    }
  }

  $header = array(
    'Navn',
    'CPR',
    'Parti',
    'Handlinger',
  );
  $form['volunteres']['#markup'] = theme('table', array('header' => $header, 'rows' => $rows));

  $form['#submit'][] = "valghalla_liste_deltager_form_submit";

  return $form;
}

/**
 * Form callback for "Deltagere med ingen meddelelser".
 */
function liste_deltager_ingen_meddelelser_form($form, &$form_state){
  $election_nid = NULL;
  if (isset($form_state['storage']['election_nid'])) {
    $election_nid = $form_state['storage']['election_nid'];
  }
  elseif (!empty($_SESSION['election_nid'])) {
    $election_nid = $_SESSION['election_nid'];
  }

  valghalla_liste_deltager_form_add_election_filter($form, $election_nid);

  $query = db_select('node', 'n')
    ->fields('n', array('nid'))
    ->orderBy('n.title')
    ->condition('n.type', 'volunteers');

  $query->leftJoin('entity_valghalla_notification', 'evn', 'n.nid = evn.vol_id');
  $query->addExpression('count(evn.vol_id)', 'count');
  $query->groupBy('n.nid');
  $query->having('count < 1');

  if (!empty($election_nid)) {
    $inner = new EntityFieldQuery();
    $fc_info = $inner->entityCondition('entity_type', 'field_collection_item')
      ->entityCondition('bundle', 'field_electioninfo')
      ->fieldCondition('field_election', 'target_id', $election_nid)
      ->propertyCondition('archived', 0)
      ->execute();

    if (!empty($fc_info['field_collection_item'])) {
      $keys = array_keys($fc_info['field_collection_item']);
      $query->leftJoin('field_data_field_electioninfo', 'fci', 'n.nid = fci.entity_id');
      $query->condition('fci.field_electioninfo_value', $keys, 'IN');
    }
  }

  $result = $query->execute()->fetchAll();
  $rows = [];
  foreach ($result as $row) {
    $volunteer = node_load($row->nid);
    $wrapper = entity_metadata_wrapper('node', $volunteer);
    $rows[$volunteer->nid] = array(
      $wrapper->title->value(),
      $wrapper->field_cpr_number->value(),
      ($wrapper->field_party->value()) ? $wrapper->field_party->value()->name : '',
      l(t('Edit'), 'node/' . $volunteer->nid . '/edit', array('query' => array('destination' => current_path()))),
    );
  }

  $header = array(
    'Navn',
    'CPR',
    'Parti',
    'Handlinger',
  );
  $form['volunteres']['#markup'] = theme('table', array('header' => $header, 'rows' => $rows));

  $form['#submit'][] = "valghalla_liste_deltager_form_submit";

  return $form;
}

/**
 * Helper function for adding election filter.
 */
function valghalla_liste_deltager_form_add_election_filter(&$form, $election_nid) {
  $form['filter'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('row')),
  );
  $form['filter']['election_nid'] = array(
    '#type' => 'select',
    '#options' => array('' => t('All')) + odin_get_elections(),
    '#default_value' => $election_nid,
    '#prefix' => '<div class="col-md-9">',
    '#suffix' => '</div>',
  );
  $form['filter']['submit'] = array(
    '#type' => 'submit',
    '#prefix' => '<div class="hidden-print col-md-3">',
    '#value' => t('Opdater'),
    '#suffix' => '</div>',
  );
}

/**
 * Helper function to react on election change - submit handler.
 */
function valghalla_liste_deltager_form_submit(&$form, &$form_state) {
  if (isset($form_state['values']['election_nid'])) {
    $form_state['storage']['election_nid'] = $form_state['values']['election_nid'];
    $form_state['rebuild'] = TRUE;
  }
}


