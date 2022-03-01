<?php

$query = new EntityFieldQuery();
$query->entityCondition('entity_type', 'node')
  ->entityCondition('bundle', 'volunteers');
$nids = array_keys(reset($query->execute()));

$field_collections = array();
$duplicates = array();
forach ($nids) as $nid) {
  $node = node_load($nid);
  $field_election_info = field_get_items('node', $node, 'field_electioninfo');
  if (empty($field_election_info)) {
    continue;
  }
  foreach ($field_election_info as $item) {
    $fc_item = entity_load_single('field_collection_item', $item['value']);
    if (empty($fc_item)) {
      continue;
    }

    $field_election = field_get_items('field_collection_item', $fc_item, 'field_election');
    if (empty($field_election[0]['target_id'])) {
      watchdog('check electioninfo duplicates', 'Empty election for fc_id ' . $fc_item->item_id);
      continue;
    }
    $field_polling_station = field_get_items('field_collection_item', $fc_item, 'field_vlnt_station');
    if (empty($field_polling_station[0]['target_id'])) {
      watchdog('check electioninfo duplicates', 'Empty polling_station for fc_id ' . $fc_item->item_id);
      continue;
    }
    $field_post_party = field_get_items('field_collection_item', $fc_item, 'field_post_party');
    if (empty($field_post_party[0]['target_id'])) {
      watchdog('check electioninfo duplicates', 'Empty post_party for fc_id ' . $fc_item->item_id);
      continue;
    }

    $field_post_role = field_get_items('field_collection_item', $fc_item, 'field_post_role');
    if (empty($field_post_role[0]['target_id'])) {
      watchdog('check post role duplicates', 'Empty post_role for fc_id ' . $fc_item->item_id);
      continue;
    }

    $key = implode('_' . array(
      $field_election[0]['target_id'],
      $field_polling_station[0]['target_id'],
      $field_post_party[0]['target_id'],
      $field_post_role[0]['target_id'],
    ));

    if (in_array($key, $field_collections[$node->nid]])) {
      $duplicates[$node->nid][] = $key;
      continue;
    }

    $field_collections[$node->nid][] = $key;
  }
}

print_r($duplicates);
