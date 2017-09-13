<?php
/**
 * @file
 * Implements the Protocol content type.
 */

/**
 * Implements hook_node_access().
 *
 * @param $node
 * THe node on which the operation is performed, or, if it does not yet exist,
 *   the type of node to be created.
 *
 * @param $op
 * The operation to be performed.
 *
 * @param $account
 * A user object representing the user for whom the operation is to be
 *   performed.
 *
 * @return
 * If the permission for the specified operation is not set then return FALSE.
 *   If the permissions is set then return NULL as this allows other modules to
 *   disable access. The only exception is when the $op == 'create'. We will
 *   always return TRUE if the permission is set.
 *
 * @ingroup tripal_protocol
 *
 */
function tripal_protocol_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }

  if ($node_type == 'chado_protocol') {
    if ($op == 'create') {
      if (!user_access('create chado_protocol content', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_protocol content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_protocol content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_protocol content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    return NODE_ACCESS_IGNORE;
  }
}

/**
 * Implements hook_node_info().
 *
 *  Provides information to drupal about the node types that we're creating
 *  in this module. This module creates four content types: Analysis:
 * Expression, arraydesign, biomaterial, and protocol.
 *
 * @ingroup tripal_analysis_expression
 */
function tripal_protocol_node_info() {
  $nodes = [];
  $nodes['chado_protocol'] = [
    'name' => t('Protocol'),
    'base' => 'chado_protocol',
    'description' => t('A protocol.'),
    'has_title' => TRUE,
    'title_label' => t('Protocol'),
    'locked' => TRUE,
    'chado_node_api' => [
      'base_table' => 'protocol',
      'hook_prefix' => 'chado_protocol',
      'record_type_title' => [
        'singular' => t('Protocol'),
        'plural' => t('Protocols'),
      ],
      'sync_filters' => [
        'type_id' => FALSE,
        'organism_id' => FALSE,
        'checkboxes' => ['name'],
      ],
    ],
  ];
  $nodes['chado_arraydesign'] = [
    'name' => t('Array design'),
    'base' => 'chado_arraydesign',
    'description' => t('Array design'),
    'has_title' => TRUE,
    'title_label' => t('Array Design'),
    'locked' => TRUE,
    'chado_node_api' => [
      'base_table' => 'arraydesign',
      'hook_prefix' => 'chado_array_design',
      'record_type_title' => [
        'singular' => t('Array design'),
        'plural' => t('Array designs'),
      ],
      'sync_filters' => [
        'type_id' => FALSE,
        'organism_id' => FALSE,
        'checkboxes' => ['name'],
      ],
    ],
  ];

  return $nodes;
}

/**
 * Implements hook_form()
 *
 * When editing or create a new node of type 'chado_protocol' we need
 * a form. This function creates the form that will be used for this.
 *
 * @ingroup tripal_protocol
 */
function chado_protocol_form($node, &$form_state) {
  $form = [];

  // Not sure what this is for. Is this line necessary?
  $form['#attribute']['enctype'] = 'multipart/form-data';

  $protocol_name = '';
  $uri = '';
  $protocoldescription = '';
  $hardwaredescription = '';
  $softwaredescription = '';
  $type_id = '';
  $pub_id = '';

  if ($node) {
    if (property_exists($node, 'protocol')) {
      $protocol = $node->protocol;

      // Fill out the form.
      $protocol_name = property_exists($node, 'protocol_name') ? property_exists($node, 'protocol_name') : $protocol->name;
      $uri = property_exists($node, 'uri') ? property_exists($node, 'uri') : $protocol->uri;
      $protocoldescription = property_exists($node, 'protocoldescription') ? property_exists($node, 'protocoldescription') : $protocol->protocoldescription;
      $hardwaredescription = property_exists($node, 'hardwaredescription') ? property_exists($node, 'hardwaredescription') : $protocol->hardwaredescription;
      $softwaredescription = property_exists($node, 'softwaredescription') ? property_exists($node, 'softwaredescription') : $protocol->softwaredescription;
      $type_id = property_exists($node, 'type_id') ? property_exists($node, 'type_id') : $protocol->type_id->cvterm_id;
      /*if ($protocol->pub_id->pub_id) {
        $pub_id = property_exists($node, 'pub_id') ? property_exists($node, 'pub_id') : $protocol->pub_id->pub_id;
      } else {
        $pub_id = property_exists($node, 'pub_id') ? property_exists($node, 'pub_id') : '';
      } */

      // The conditional here is required because this field is chosen by an optional select
      if ($protocol->pub_id != NULL) {
        $pub_id = property_exists($node, 'pub_id') ? property_exists($node, 'pub_id') : $protocol->pub_id->pub_id;
      }
      else {
        $pub_id = NULL;
      }

      // Set the protocol id.
      $form['protocol_id'] = [
        '#type' => 'value',
        '#value' => $protocol->protocol_id,
      ];
      $protocol_id = $protocol->protocol_id;
    }

    else {
      $protocol_name = property_exists($node, 'protocol_name') ? property_exists($node, 'protocol_name') : '';
      $uri = property_exists($node, 'uri') ? property_exists($node, 'uri') : '';
      $protocoldescription = property_exists($node, 'protocoldescription') ? property_exists($node, 'protocoldescription') : '';
      $hardwaredescription = property_exists($node, 'hardwaredescription') ? property_exists($node, 'hardwaredescription') : '';
      $softwaredescription = property_exists($node, 'softwaredescription') ? property_exists($node, 'softwaredescription') : '';
      $type_id = property_exists($node, 'type_id') ? property_exists($node, 'type_id') : '';
      $pub_id = property_exists($node, 'pub_id') ? property_exists($node, 'pub_id') : '';

      $protocol_id = NULL;
    }
  }

  $form['protocol_name'] = [
    '#type' => 'textfield',
    '#title' => t('Protocol Name (must be unique)'),
    '#required' => TRUE,
    '#default_value' => $protocol_name,
  ];
  $form['uri'] = [
    '#type' => 'textfield',
    '#title' => t('Protocol link'),
    '#default_value' => $uri,
  ];
  $form['protocoldescription'] = [
    '#type' => 'textarea',
    '#rows' => 7,
    '#title' => t('Protocol Description'),
    '#default_value' => $protocoldescription,
  ];
  $form['hardwaredescription'] = [
    '#type' => 'textarea',
    '#rows' => 7,
    '#title' => t('Hardware Description'),
    '#default_value' => $hardwaredescription,
  ];
  $form['softwaredescription'] = [
    '#type' => 'textarea',
    '#rows' => 7,
    '#title' => t('Software Description'),
    '#default_value' => $softwaredescription,
  ];

  // Obtain the list of protocol types from the cv table protocol_type.
  $sql = 'SELECT
            CVT.cvterm_id, CVT.name 
          FROM {cvterm} CVT
            INNER JOIN {cv} CV ON CVT.cv_id = CV.cv_id
          WHERE 
            CV.name = :name 
          ORDER BY CVT.name';
  $results = chado_query($sql, [':name' => 'protocol_type']);
  $types = [];
  while ($type = $results->fetchObject()) {
    $types[$type->cvterm_id] = $type->name;
  }

  $form['choose_type'] = [
    '#type' => 'value',
    '#value' => $types,
  ];

  // Get the cv_id of protocol_type. This is necessary to create the 
  // correct link in the platform form description so the user can create a new 
  // protocol_type cv_term.  
  $cv_id = tripal_get_cv(['name' => 'protocol_type'])->cv_id;

  $lt_message = tripal_set_message(t('If the protocol type does not appear in the this list, ') . l(t('add a new protocol type'), 'admin/tripal/chado/tripal_cv/cv/' . $cv_id . '/cvterm/add') . t(' to the protocol_type controlled vocabulary. A protocol type is required'), TRIPAL_INFO, ['return_html' => TRUE]);

  $form['type_id'] = [
    '#title' => t('Protocol Type'),
    '#type' => 'select',
    '#required' => TRUE,
    '#empty_value' => '',
    '#description' => t('The protocol type.'),
    '#options' => $form['choose_type']['#value'],
    '#default_value' => $type_id,
    '#suffix' => $lt_message,
  ];

  // Obtain the list of contacts from the chado contact table. 
  $sql = 'SELECT
            pub_id, title 
          FROM {pub} 
          ORDER BY title';
  $results = chado_query($sql);
  $pubs = [];
  while ($pub = $results->fetchObject()) {
    $pubs[$pub->pub_id] = $pub->title;
  }

  $form['choose_pub'] = [
    '#type' => 'value',
    '#value' => $pubs,
  ];

  $lt_message = tripal_set_message(t('If the publication does not appear in this list, ') . l(t('add a new publication.'), 'node/add/chado-pub') . t(' A publication is not required.'), TRIPAL_INFO, ['return_html' => TRUE]);

  $form['pub_id'] = [
    '#title' => t('Publication'),
    '#type' => 'select',
    '#empty_value' => '',
    '#description' => t('The publication associated with this protocol.'),
    '#options' => $form['choose_pub']['#value'],
    '#default_value' => $pub_id,
    '#suffix' => $lt_message,
  ];

  return $form;
}

/**
 * Implements of hook_validate().
 *
 * @ingroup tripal_protocol
 */
function chado_protocol_validate($node, $form, &$form_state) {
  // Don't validate if a delete.
  if ($node->op == 'Delete') {
    return;
  }

  // We are syncing if there is an protocol_id but no node ID. In this case a 
  // validation is not required.  
  if (is_null($node->nid) and property_exists($node, 'protocol_id') and $node->protocol_id != 0) {
    return;
  }

  // Remove white space around field values.
  $node->protocol_name = trim($node->protocol_name);
  $node->uri = trim($node->uri);
  $node->protocoldescription = trim($node->protocoldescription);
  $node->hardwaredescription = trim($node->hardwaredescription);
  $node->softwaredescription = trim($node->softwaredescription);

  // Validate for an update.
  if (property_exists($node, 'protocol_id')) {
    $sql = "
      SELECT * 
      FROM {protocol} 
      WHERE
        name = :name AND NOT
        protocol_id = :protocol_id
    ";
    $args = [
      ':name' => $node->protocol_name,
      ':protocol_id' => $node->protocol_id,
    ];
    $result = chado_query($sql, $args)->fetchObject();
    if ($result) {
      form_set_error('name', t("Update cannot proceed. The protocol
        '$node->protocol_name' is already present in the database."));
      tripal_report_error('tripal_analysis_expression', TRIPAL_WARNING, 'Update protocol: name already exists: %value', ['%value' => "name = $node->protocol_name"]);
    }
  }
  // Validate for an insert.
  else {
    $value = [
      'name' => $node->protocol_name,
    ];
    $protocol = chado_select_record('protocol', ['protocol_id'], $value);
    if (sizeof($protocol) > 0) {
      form_set_error('protocol_name', 'Cannot add the protocol with this name. 
        The protocol already exists.');
      tripal_report_error('tripal_analysis_expression', TRIPAL_WARNING, 'Insert protocol: name already exists: %value', ['%value' => "name = $node->protocol_name"]);
    }
  }
}

/**
 * Implements hook_insert().
 *
 * When the new chado_protocol is created we also need to add information
 * to the chado_protocol table. This function is called on insert of a new node
 * of type 'chado_protocol' and inserts the necessary information.
 *
 * @ingroup tripal_protocol
 */
function chado_protocol_insert($node) {

  // Remove white space around field values.
  $node->protocol_name = trim($node->protocol_name);
  $node->uri = trim($node->uri);
  $node->protocoldescription = trim($node->protocoldescription);
  $node->hardwaredescription = trim($node->hardwaredescription);
  $node->softwaredescription = trim($node->softwaredescription);

  if ($node->pub_id == "") {
    $pub_id = NULL;
  }
  else {
    $pub_id = $node->pub_id;
  }

  // Check to see if the node is already associated with a chado record. 
  if (!property_exists($node, 'protocol_id')) {
    $values = [
      'name' => $node->protocol_name,
      'uri' => $node->uri,
      'protocoldescription' => $node->protocoldescription,
      'hardwaredescription' => $node->hardwaredescription,
      'softwaredescription' => $node->softwaredescription,
      'type_id' => $node->type_id,
      'pub_id' => $pub_id,
    ];

    /*  if ($node->pub_id) {
        $values['pub_id'] = $node->pub_id;
      } */

    $protocol = chado_insert_record('protocol', $values);
    if (!$protocol) {
      drupal_set_message(t('Unable to add protocol.', 'warning'));
      tripal_report_error('tripal_analysis_expression', TRIPAL_ERROR, 'Insert Protocol: Unable to create protocol where values: %values', ['%values' => print_r($values, TRUE)]);
      return;
    }
    $protocol_id = $protocol['protocol_id'];
  }
  else {
    $protocol_id = $node->protocol_id;
  }

  // Check to see if the chado entry has a corresponding chado_protocol entry.
  // Add if no corresponding entry exists in the chado_protocol table. 
  $check_org_id = chado_get_id_from_nid('protocol', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->protocol_id = $protocol_id;
    drupal_write_record('chado_protocol', $record);
  }
}

/**
 * Implements hook_load().
 *
 *  When a node is requested by the user this function is called to allo us
 *  to add auxiliary data to the node object.
 *
 * @ingroup tripal_arraydesign
 */
function chado_protocol_load($nodes) {

  foreach ($nodes as $nid => $node) {
    // Find the arraydesign nodes and add details. 
    $protocol_id = chado_get_id_from_nid('protocol', $nid);

    // Ignore orphaned nodes.
    if (!$protocol_id) {
      continue;
    }

    // Build the arraydesign variable.
    $values = ['protocol_id' => $protocol_id];
    $protocol = chado_generate_var('protocol', $values);

    // Add the description field,
    //$arraydesign = chado_expand_var($arraydesign, 'field', 'arraydesign.description');
    $nodes[$nid]->protocol = $protocol;

    // Get title the node title.
    $node->title = chado_get_node_title($node);
  }
}

/**
 * Implements [content_type]_chado_node_default_title_format().
 *
 * Defines a default title format for the Chado Node API to set the titles on
 * chado_arraydesign ndoes based on chado fields.
 */
function chado_protocol_chado_node_default_title_format() {
  return '[protocol.name]';
}

/**
 * Implements hook_chado_node_default_url_format().
 *
 * Defines a default URL format for arraydesign nodes.
 */
function chado_protocol_chado_node_default_url_format() {
  return '/protocol/[protocol.name]';
}

/**
 * Implements hook_node_insert().
 * Acts on all content types.
 *
 * @ingroup tripal_protocol
 */
function tripal_protocol_node_insert($node) {

  switch ($node->type) {
    case 'chado_protocol':

      // Find the a protocol and add details.     
      $protocol_id = chado_get_id_from_nid('protocol', $node->nid);
      $values = ['protocol_id' => $protocol_id];
      $protocol = chado_generate_var('protocol', $values);
      $node->protocol = $protocol;

      // Set the node title.  
      $node->title = chado_get_node_title($node);

      // Set the URL path.  
      chado_set_node_url($node);

      break;
  }
}

/**
 * Implements hook_node_update().
 * Acts on all content types.
 *
 * @ingroup tripal_protocol
 */
function tripal_protocol_node_update($node) {

  switch ($node->type) {
    case 'chado_protocol':

      // Set the node title.  
      $node->title = chado_get_node_title($node);

      // Set the URL path.  
      chado_set_node_url($node);

      break;
  }
}

/**
 * Implements hook_update().
 *
 * @ingroup tripal_arraydesign
 */
function chado_protocol_update($node) {

  // Remove white space around field values.
  $node->protocol_name = trim($node->protocol_name);
  $node->uri = trim($node->uri);
  $node->protocoldescription = trim($node->protocoldescription);
  $node->hardwaredescription = trim($node->hardwaredescription);
  $node->softwaredescription = trim($node->softwaredescription);

  $protocol_id = chado_get_id_from_nid('protocol', $node->nid);

  if ($node->revision) {
    // Chado does not do revisions but following the example of prior modules
    // keep this here for when/if revisions are addressed in the future. 
  }
  $match = [
    'protocol_id' => $protocol_id,
  ];
  $values = [
    'name' => $node->protocol_name,
    'uri' => $node->uri,
    'protocoldescription' => $node->protocoldescription,
    'hardwaredescription' => $node->hardwaredescription,
    'softwaredescription' => $node->softwaredescription,
    'type_id' => $node->type_id,
    'pub_id' => $node->pub_id,
  ];

  $status = chado_update_record('protocol', $match, $values);

  if (!$status) {
    drupal_set_message(t('Unable to update protocol.'), 'warning');
    tripal_report_error('tripal_analysis_expression', TRIPAL_WARNING, 'Update protocol: Unable to update protocol where values: %values', ['%values' => print_r($values, TRUE)]);
  }
}

/**
 * Implements hook_delete().
 *
 * Delete protocol from both drupal and chado databases. Check dependencies
 * before deleting from chado.
 *
 * @ingroup tripal_protocol
 */
function chado_protocol_delete($node) {
  $protocol_id = chado_get_id_from_nid('protocol', $node->nid);

  // If there is no protocol for this node, this is not the correct type or the chado
  // entry is lost. 
  if (!$protocol_id) {
    return;
  }

  // Remove data from drupal tables including chado_protocol, node, and node_revisions
  $sql_del = "DELETE FROM {chado_protocol} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, [':nid' => $node->nid, ':vid' => $node->vid]);
  $sql_del = "DELETE FROM {node} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, [':nid' => $node->nid, ':vid' => $node->vid]);
  $sql_del = "DELETE FROM {node_revision} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, [':nid' => $node->nid, ':vid' => $node->vid]);

  // Check for dependencies before deleting from the chado database.
  // A dependency is the element table. More dependencies may need to be checked
  // in the future if functionality is extended. 

  $sql = "SELECT method_id FROM {:table} WHERE protocol_id = :protocol_id";
  $check_acquisition = chado_query('SELECT acquisition_id FROM {acquisition} WHERE protocol_id = :protocol_id', [':protocol_id' => $protocol_id])->fetchObject();
  $check_arraydesign = chado_query('SELECT arraydesign_id FROM {arraydesign} WHERE protocol_id = :protocol_id', [':protocol_id' => $protocol_id])->fetchObject();
  $check_assay = chado_query('SELECT assay_id FROM {assay} WHERE protocol_id = :protocol_id', [':protocol_id' => $protocol_id])->fetchObject();
  $check_quantification = chado_query('SELECT quantification_id FROM {quantification} WHERE protocol_id = :protocol_id', [':protocol_id' => $protocol_id])->fetchObject();
  $check_treatment = chado_query('SELECT treatment_id FROM {treatment} WHERE protocol_id = :protocol_id', [':protocol_id' => $protocol_id])->fetchObject();
  $check_protocolparam = chado_query('SELECT protocolparam_id FROM {protocolparam} WHERE protocol_id = :protocol_id', [':protocol_id' => $protocol_id])->fetchObject(); //*** Might need to get rid of this one - not sure

  if (!$check_acquisition or !$check_arraydesign or !$check_assay or !$check_quantification or !$check_treatment or !$check_protocolparam) {
    chado_delete_record('protocol', ['protocol_id' => $protocol_id]);
  }
  else {
    drupal_set_message(t("Warning: other data depends on this protocol. The protocol page was removed from this site but the protocol record was not removed from Chado."), 'warning');
  }
} 

