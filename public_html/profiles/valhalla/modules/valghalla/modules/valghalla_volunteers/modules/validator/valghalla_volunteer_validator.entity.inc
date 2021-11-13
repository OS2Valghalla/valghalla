<?php

/**
 * @file
 * Handles the Valghalla validation record entity.
 */

/**
 * Implements hook_entity_info().
 */
function valghalla_volunteer_validator_entity_info() {
  $info['valghalla_volunteer_validation_record'] = array(
    'label' => t('Valghalla Volunteer Validation Record Entity'),
    'controller class' => 'ValghallaVolunteerValidationRecordController',
    'entity class' => 'ValghallaVolunteerValidationRecord',
    'base table' => 'valghalla_volunteer_validation_record',
    'uri callback' => 'valghalla_volunteer_validation_record_uri',
    'entity keys' => array(
      'id' => 'id',
    ),
    'bundle' => 'bundle_type',
    'bundle keys' => array(
      'bundle' => 'bundle_type',
    ),
    'bundles' => array(
      'default_bundle' => array(
        'label' => 'Default validation record bundle',
      ),
    ),
    'static cache' => TRUE,
    'view modes' => array(
      'default' => array(
        'label' => t('Default'),
        'custom settings' => FALSE,
      ),
    ),
  );

  return $info;
}

/**
 * Fetch a valghalla_volunteer_validator object.
 *
 * This function ends up being a shim between the menu system and
 * valghalla_volunteer_validation_record_load_multiple().
 *
 * @param int $id
 *   Integer specifying the valghalla_volunteer_validator entity id.
 * @param bool $reset
 *   A boolean indicating that the internal cache should be reset.
 *
 * @return mixed
 *   A fully-loaded entity object or FALSE if it cannot be loaded.
 *
 * @see valghalla_volunteer_validation_record_load_multiple()
 */
function valghalla_volunteer_validation_record_load($id = NULL, $reset = FALSE) {
  $ids = (isset($id) ? array($id) : array());
  $valghalla_volunteer_validator = valghalla_volunteer_validation_record_load_multiple($ids, array(), $reset);
  return $valghalla_volunteer_validator ? reset($valghalla_volunteer_validator) : FALSE;
}

/**
 * Loads multiple valghalla_volunteer_validator entities.
 *
 * We only need to pass this request along to entity_load(), which
 * will in turn call the load() method of our entity controller class.
 */
function valghalla_volunteer_validation_record_load_multiple($ids = FALSE, $conditions = array(), $reset = FALSE) {
  return entity_load('valghalla_volunteer_validation_record', $ids, $conditions, $reset);
}

/**
 * Implements the uri callback.
 */
function valghalla_volunteer_validation_record_uri($valghalla_volunteer_validator) {
  return array(
    'path' => 'valghalla/administration/volunteer-validation-records/' . $valghalla_volunteer_validator->id,
  );
}

/**
 * Defines Valghalla Volunteer Validation Record Entity menu items.
 */
function valghalla_volunteer_validation_record_menu(&$items) {
  $items['valghalla/administration/volunteer-validation-records-list'] = array(
    'title' => 'Deltagere validering list',
    'page callback' => 'valghalla_volunteer_validation_record_list_entities',
    'access arguments' => array('view any valghalla_volunteer_validation_record entity'),
    'type' => MENU_NORMAL_ITEM,
  );
  $items['valghalla/administration/volunteer-validation-records-list/%'] = array(
    'title' => 'Deltagere validering list',
    'page callback' => 'valghalla_volunteer_validation_record_list_entities',
    'page arguments' => array(3),
    'access arguments' => array('view any valghalla_volunteer_validation_record entity'),
    'type' => MENU_NORMAL_ITEM,
  );

  $items['valghalla_lists/volunteers_validation'] = array(
    'title' => 'Deltagere validering',
    'page callback' => 'drupal_goto',
    'page arguments' => array('valghalla/administration/volunteer-validation-records-list'),
    'access arguments' => array('view any valghalla_volunteer_validation_record entity'),
    'type' => MENU_NORMAL_ITEM,
  );

  $items['valghalla/administration/volunteer-validation-records/%valghalla_volunteer_validation_record'] = array(
    'title callback' => 'valghalla_volunteer_validation_record_title',
    'title arguments' => array(3),
    'page callback' => 'valghalla_volunteer_validation_record_view',
    'page arguments' => array(3),
    'access arguments' => array('view any valghalla_volunteer_validation_record entity'),
  );

  // 'View' tab for an individual entity page.
  $items['valghalla/administration/volunteer-validation-records/%valghalla_volunteer_validation_record/view'] = array(
    'title' => 'View',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => -10,
  );

  // 'Remove' tab for an individual entity page.
  $items['valghalla/administration/volunteer-validation-records/%valghalla_volunteer_validation_record/remove'] = array(
    'title' => 'Remove',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('valghalla_volunteer_validation_record_confirm_delete', 3),
    'access arguments' => array('administer valghalla_volunteer_validation_record entities'),
    'type' => MENU_LOCAL_TASK,
  );

  $items['node/%/validation-records'] = array(
    'title callback' => 'valghalla_volunteer_validation_record_tab_title',
    'page callback' => 'valghalla_volunteer_validation_record_election_node_redirect',
    'page arguments' => array(1),
    'access callback' => 'valghalla_volunteer_validation_record_check_tab_access',
    'type' => MENU_LOCAL_TASK,
    'weight' => 10,
  );

  return $items;
}

/**
 * Returns a render array with all entities.
 */
function valghalla_volunteer_validation_record_list_entities($election_id = NULL, $vol_nid = FALSE) {
  $content = array();

  // Load all of our entities.
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'valghalla_volunteer_validation_record')
    ->propertyOrderBy('created', 'DESC')
    ->pager(20);
  $headers[] = t('ID');
  if ($vol_nid) {
    $query->propertyCondition('vol_id', $vol_nid);
  }
  else {
    $headers[] = t('Deltager');
  }

  if (!isset($election_id) && !empty($_SESSION['election_nid'])) {
    $election_id = $_SESSION['election_nid'];
  }
  if (!empty($election_id) && $election_id != 'all') {
    $query->propertyCondition('election_id', $election_id);
  }

  $content['links'] = valghalla_volunteer_validation_record_counter_links($election_id);

  $headers[] = t('Type');
  $headers[] = t('Besked');
  $headers[] = t('Tidspunkt');
  $headers[] = t('Operations');
  $result = $query->execute();
  $types_labels = valghalla_volunteer_validation_record_get_types();
  $ids = reset($result);
  if (!empty($ids)) {
    $entities = valghalla_volunteer_validation_record_load_multiple(array_keys($ids));
    foreach ($entities as $entity) {
      $message = strip_tags(str_replace('<br/>', '  ', $entity->message));
      $row = $row_data = array();
      $row_data['id'] = l($entity->id, 'valghalla/administration/volunteer-validation-records/' . $entity->id);
      if (!$vol_nid) {
        $row_data['volunteer'] = valghalla_volunteer_validation_record_node_reference_view($entity->vol_id);
      }
      $row_data['type'] = $types_labels[$entity->type];
      $row_data['message'] = check_markup($message, 'plain_text');
      $row_data['created'] = format_date($entity->created, 'short');
      $row_data['operations'] = theme('links__ctools_dropbutton', array(
        'links' => array(
          array('title' => t('View'), 'href' => 'valghalla/administration/volunteer-validation-records/' . $entity->id),
          array('title' => t('Remove'), 'href' => 'valghalla/administration/volunteer-validation-records/' . $entity->id . '/remove'),
        ),
      ));

      $row['data'] = $row_data;
      $rows[] = $row;
    }
    // Put our entities into a themed table. See theme_table() for details.
    $content['entity_table'] = array(
      '#theme' => 'table',
      '#rows' => $rows,
      '#header' => $headers,
    );
  }
  else {
    // There were no entities. Tell the user.
    $content[] = array(
      '#type' => 'item',
      '#markup' => t('Ingen resultater.'),
    );
  }
  $content['pager'] = array('#theme' => 'pager');
  return $content;
}

/**
 * Get validation record counters.
 */
function valghalla_volunteer_validation_record_counter_links($active_election_id = NULL) {
  $items = array();

  $query = db_select('valghalla_volunteer_validation_record', 'v')
    ->fields('v', array('election_id'));
  $query->groupBy('v.election_id');
  $query->addExpression('count(*)', 'count');

  $result = $query->execute()->fetchAll();
  if (empty($result)) {
    return $items;
  }

  $all = 0;
  foreach ($result as $row) {
    $election = node_load($row->election_id);
    $all += $row->count;
    if (empty($election) || $election->type != 'election') {
      continue;
    }

    $items[] = array(
      'data' => l($election->title . ' (' . $row->count . ')', 'valghalla/administration/volunteer-validation-records-list/' . $election->nid, array('query' => drupal_get_destination())),
      'class' => $election->nid == $active_election_id ? array('active') : NULL,
    );
  }
  array_unshift($items, array(
    'data' => l(t('All') . ' (' . $all . ')', 'valghalla/administration/volunteer-validation-records-list/all', array('query' => drupal_get_destination())),
    'class' => ('all' == $active_election_id || empty($active_election_id)) ? array('active') : NULL,
  ));
  return array(
    '#theme' => 'item_list',
    '#items' => $items,
    '#attributes' => array('class' => 'tabs--primary nav nav-tabs'),
  );
}

/**
 * Callback for a page title when this entity is displayed.
 */
function valghalla_volunteer_validation_record_title($entity) {
  return t('Validering optage (id=@id)', array('@id' => $entity->id));
}

/**
 * Callback for election node local task menu item.
 */
function valghalla_volunteer_validation_record_election_node_redirect($election_id) {
  drupal_goto('valghalla/administration/volunteer-validation-records-list/' . $election_id);
}

/**
 * Checks access to validation records tab.
 */
function valghalla_volunteer_validation_record_check_tab_access() {
  $node = node_load(arg(1));
  if (($node->type == 'election')
    && user_access('view any valghalla_volunteer_validation_record entity')) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Generates validation record tab title with counter.
 */
function valghalla_volunteer_validation_record_tab_title() {
  $query = new EntityFieldQuery();
  $ids = $query->entityCondition('entity_type', 'valghalla_volunteer_validation_record')
    ->propertyOrderBy('created', 'DESC')
    ->propertyCondition('election_id', arg(1))
    ->execute();
  $count = empty($ids) ? 0 : count(reset($ids));
  return t('Deltagere validering log (@count)', array('@count' => $count));
}

/**
 * Menu callback to display an entity.
 *
 * As we load the entity for display, we're responsible for invoking a number
 * of hooks in their proper order.
 *
 * @see hook_entity_prepare_view()
 * @see hook_entity_view()
 * @see hook_entity_view_alter()
 */
function valghalla_volunteer_validation_record_view($entity, $view_mode = 'default') {
  // Our entity type, for convenience.
  $entity_type = 'valghalla_volunteer_validation_record';
  // Start setting up the content.
  $entity->content = array(
    '#view_mode' => $view_mode,
  );
  // Build fields content - this is where the Field API really comes in to play.
  // The task has very little code here because it all gets taken care of by
  // field module.
  // field_attach_prepare_view() lets the fields load any data they need
  // before viewing.
  field_attach_prepare_view($entity_type, array($entity->id => $entity),
    $view_mode);
  // We call entity_prepare_view() so it can invoke hook_entity_prepare_view()
  // for us.
  entity_prepare_view($entity_type, array($entity->id => $entity));
  // Now field_attach_view() generates the content for the fields.
  $entity->content += field_attach_view($entity_type, $entity, $view_mode);

  $entity->content['created'] = array(
    '#type' => 'item',
    '#title' => t('Afsendelses dato'),
    '#markup' => format_date($entity->created),
  );
  $entity->content['message'] = array(
    '#type' => 'item',
    '#title' => t('Besked'),
    '#markup' => check_markup($entity->message, 'plain_text'),
  );
  $entity->content['volunteer'] = array(
    '#type' => 'item',
    '#title' => t('Deltager'),
    '#markup' => valghalla_volunteer_validation_record_node_reference_view($entity->vol_id),
  );
  $entity->content['election'] = array(
    '#type' => 'item',
    '#title' => t('Valg'),
    '#markup' => valghalla_volunteer_validation_record_node_reference_view($entity->election_id),
  );

  // Now to invoke some hooks. We need the language code for
  // hook_entity_view(), so let's get that.
  global $language;
  $langcode = $language->language;

  // And now invoke hook_entity_view().
  module_invoke_all('entity_view', $entity, $entity_type, $view_mode,
    $langcode);
  // Now invoke hook_entity_view_alter().
  drupal_alter(array('valghalla_volunteer_validator_view', 'entity_view'),
    $entity->content, $entity_type);

  // And finally return the content.
  return $entity->content;
}

/**
 * Implements hook_field_extra_fields().
 *
 * This exposes the "extra fields" (usually properties that can be configured
 * as if they were fields) of the entity as pseudo-fields
 * so that they get handled by the Entity and Field core functionality.
 * Node titles get treated in a similar manner.
 */
function valghalla_volunteer_validator_field_extra_fields() {
  $form_elements = array(
    'type' => array('label' => t('Type')),
    'vol_id' => array('label' => t('Deltager id')),
    'election_id' => array('label' => t('Valg id')),
    'message' => array('label' => t('Besked')),
  );
  $display_elements = array(
    'type' => array('label' => t('Type')),
    'vol_id' => array('label' => t('Deltager id')),
    'election_id' => array('label' => t('Valg id')),
    'message' => array('label' => t('Besked')),
    'created' => array('label' => t('Opretelses dato')),
  );

  // Since we have only one bundle type, we'll just provide the extra_fields
  // for it here.
  $extra_fields['valghalla_volunteer_validation_record']['default_bundle']['form'] = $form_elements;
  $extra_fields['valghalla_volunteer_validation_record']['default_bundle']['display'] = $display_elements;

  return $extra_fields;
}

/**
 * Form builder for the valghalla_volunteer_validator delete confirmation form.
 */
function valghalla_volunteer_validation_record_confirm_delete($form, &$form_state, $record) {
  $form['record_id'] = array('#type' => 'value', '#value' => $record->id);
  $form_state['storage']['record'] = $record;
  return confirm_form($form,
    t('Delete validation record'),
    'admin/structure/valghalla_volunteer_validation_record/manage',
    t('Are you sure you want to delete the validation record %id ?', array('%id' => $record->id)));
}

/**
 * Submit handler to delete a valghalla_volunteer_validator after confirmation.
 */
function valghalla_volunteer_validation_record_confirm_delete_submit($form, &$form_state) {
  $form_state['storage']['record']->delete();
  drupal_set_message(t('Deleted validation record %id.', array('%id' => $form_state['values']['record_id'])));
  $form_state['redirect'] = 'valghalla/administration/volunteer-validation-records';
}

/**
 * View function for node reference id value.
 */
function valghalla_volunteer_validation_record_node_reference_view($nid) {
  $node = node_load($nid);
  $output = empty($node) ? 'N/A' : l($node->title, 'node/' . $node->nid, array(
    'attributes' => array('target' => '_blank'),
  ));
  return $output;
}

/**
 * View function for node reference id value.
 */
function valghalla_volunteer_validation_record_get_types() {
  return array(
    'external_server_sync' => 'Ekstern tilmelding',
    'no_digital_post' => 'Mangler DP oplysninger',
    'no_nemsms' => 'Mangler Nem SMS oplysninger',
  );
}
