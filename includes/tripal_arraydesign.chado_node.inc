<?php
/**
 * @file
 * Implements the Array Design content type. 
 */

/**
 * Implements hook_node_access().
 * 
 * @param $node
 * THe node on which the operation is performed, or, if it does not yet exist, the 
 * type of node to be created.
 *
 * @param $op
 * The operation to be performed. 
 * 
 * @param $account
 * A user object representing the user for whom the operation is to be performed. 
 *
 * @return 
 * If the permission for the specified operation is not set then return FALSE. If the
 * permissions is set then return NULL as this allwos other modules to disable
 * access. The only exception is when the $op == 'create'. We will always
 * return TRUE if the permission is set. 
 * 
 * @ingroup tripal_arraydesign 
 * 
 */
function chado_arraydesign_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }

  if ($node_type == 'chado_arraydesign') {
    if ($op == 'create') {
      if (!user_access('create chado_arraydesign content', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_arraydesign content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_arraydesign content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_arraydesign content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    return NODE_ACCESS_IGNORE;
  }
}

/**
 * Implements hook_form()
 * 
 * When editing or create a new node of type 'chado_arraydesign' we need
 * a form. This function creates the form that will be used for this. 
 * 
 * @ingroup tripal_arraydesign
 */
function chado_arraydesign_form($node, $form_state) {
  $form = array();
 
  // Not sure what this is for. Is this line necessary? 
  $form['#attribute']['enctype'] = 'multipart/form-data';
  
  if (property_exists($node, 'arraydesign')) {   
    $arraydesign = $node->arraydesign;

    // Fill out the form.
    $arraydesign_name = property_exists($node, 'arraydesign_name') ? property_exists($node, 'arraydesign_name') : $arraydesign->name;
    $version = property_exists($node, 'version') ? property_exists($node, 'version') : $arraydesign->version;
    $description = property_exists($node, 'description') ? property_exists($node, 'description') : $arraydesign->description;
    $array_dimensions = property_exists($node, 'array_dimensions') ? property_exists($node, 'array_dimensions') : $arraydesign->array_dimensions;
    $element_dimensions = property_exists($node, 'element_dimensions') ? property_exists($node, 'element_dimensions') : $arraydesign->element_dimensions;
    $num_of_elements = property_exists($node, 'num_of_elements') ? property_exists($node, 'num_of_elements') : $arraydesign->num_of_elements;
    $num_array_columns = property_exists($node, 'num_array_columns') ? property_exists($node, 'num_array_columns') : $arraydesign->num_array_columns;
    $num_array_rows = property_exists($node, 'num_array_rows') ? property_exists($node, 'num_array_rows') : $arraydesign->num_array_rows;
    $num_grid_columns = property_exists($node, 'num_grid_columns') ? property_exists($node, 'num_grid_columns') : $arraydesign->num_grid_columns;
    $num_grid_rows = property_exists($node, 'num_grid_rows') ? property_exists($node, 'num_grid_rows') : $arraydesign->num_grid_rows;
    $num_sub_columns = property_exists($node, 'num_sub_columns') ? property_exists($node, 'num_sub_columns') : $arraydesign->num_sub_columns;
    $num_sub_rows = property_exists($node, 'num_sub_rows') ? property_exists($node, 'num_sub_rows') : $arraydesign->num_sub_rows;
    $manufacturer_id = property_exists($node, 'manufacturer_id') ? property_exists($node, 'manufacturer_id') : $arraydesign->manufacturer_id->contact_id;
    $platformtype_id = property_exists($node, 'platformtype_id') ? property_exists($node, 'platformtype_id') : $arraydesign->platformtype_id->cvterm_id;

    // The conditional here is required because this field is chosen by an optional select
    if ($arraydesign->protocol_id != NULL) {
      $protocol_id = property_exists($node, 'protocol_id') ? property_exists($node, 'protocol_id') : $arraydesign->protocol_id->protocol_id;
    } 
    else { 
      $protocol_id = NULL; 
    }

    // The conditional here is required because this field is chosen by an optional select
    if ($arraydesign->substratetype_id != NULL) {
      $substratetype_id = property_exists($node, 'substratetype_id') ? property_exists($node, 'substratetype_id') : $arraydesign->substratetype_id->cvterm_id;
    }
    else {
      $substratetype_id = NULL;
    } 
   
    // Set the arraydesign id. 
    $form['arraydesign_id'] = array(
      '#type' => 'value',
      '#value' => $arraydesign->arraydesign_id,
    );
    $arraydesign_id = $arraydesign->arraydesign_id;
  }

  else {
    $arraydesign_name = property_exists($node, 'arraydesign_name') ? property_exists($node, 'arraydesign_name') : '';
    $version = property_exists($node, 'version') ? property_exists($node, 'version') : '';
    $description = property_exists($node, 'description') ? property_exists($node, 'description') : '';
    $array_dimensions = property_exists($node, 'array_dimensions') ? property_exists($node, 'array_dimensions') : '';
    $element_dimensions = property_exists($node, 'element_dimensions') ? property_exists($node, 'element_dimensions') : '';
    $num_of_elements = property_exists($node, 'num_of_elements') ? property_exists($node, 'num_of_elements') : '';
    $num_array_columns = property_exists($node, 'num_array_columns') ? property_exists($node, 'num_array_columns') : '';
    $num_array_rows = property_exists($node, 'num_array_rows') ? property_exists($node, 'num_array_rows') : '';
    $num_grid_columns = property_exists($node, 'num_grid_columns') ? property_exists($node, 'num_grid_columns') : '';
    $num_grid_rows = property_exists($node, 'num_grid_rows') ? property_exists($node, 'num_grid_rows') : '';
    $num_sub_columns = property_exists($node, 'num_sub_columns') ? property_exists($node, 'num_sub_columns') : '';
    $num_sub_rows = property_exists($node, 'num_sub_rows') ? property_exists($node, 'num_sub_rows') : '';
    $manufacturer_id = property_exists($node, 'manufacturer_id') ? property_exists($node, 'manufacturer_id') : '';
    $protocol_id = property_exists($node, 'protocol_id') ? property_exists($node, 'protocol_id') : '';
    $platformtype_id = property_exists($node, 'platformtype_id') ? property_exists($node, 'platformtype_id') : '';
    $substratetype_id = property_exists($node, 'substratetype_id') ? property_exists($node, 'substratetype_id') : '';
    
   $arraydesign_id = NULL;
  }
 
  $form['arraydesign_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Array Design Name (must be unique)'),
    '#required' => TRUE,
    '#default_value' =>  $arraydesign_name,
  );
  $form['version'] = array( 
    '#type' => 'textfield',
    '#title' => t('Array Design Version'), 
    '#default_value' => $version,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#rows' => 15,
    '#title' => t('Description'),
    '#default_value' => $description,
  );
  $form['array_dimensions'] = array(
    '#type' => 'textfield',
    '#title' => t('Array Dimensions'),
    '#default_value' => $array_dimensions, 
  );
  $form['element_dimensions'] = array(
    '#type' => 'textfield',
    '#title' => t('Element Dimensions'),
    '#default_value' => $element_dimensions, 
  );  
  $form['num_of_elements'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of Elements'),
    '#default_value' => $num_of_elements,
    '#element_validate' => array('element_validate_integer_positive'),
  );  
  $form['num_array_columns'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of Array Columns'),
    '#default_value' => $num_array_columns, 
    '#element_validate' => array('element_validate_integer_positive'),
  );  
  $form['num_array_rows'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of Array Rows'),
    '#default_value' => $num_array_rows, 
    '#element_validate' => array('element_validate_integer_positive'),
  );  
  $form['num_grid_columns'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of Grid Columns'),
    '#default_value' => $num_grid_columns, 
    '#element_validate' => array('element_validate_integer_positive'),
  );  
  $form['num_grid_rows'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of Grid Rows'),
    '#default_value' => $num_grid_rows, 
    '#element_validate' => array('element_validate_integer_positive'),
  );  
  $form['num_sub_columns'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of Sub Columns'),
    '#default_value' => $num_sub_columns, 
    '#element_validate' => array('element_validate_integer_positive'),
  );  
  $form['num_sub_rows'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of Sub Rows'),
    '#default_value' => $num_sub_rows, 
    '#element_validate' => array('element_validate_integer_positive'),
  );

  $lt_message = tripal_set_message(t('If the array manufacturer is not present in the dropdown list, ')
    . l(t('add a new contact.'), 'node/add/chado-contact') . t(' An array manufacturer is required.'),
    TRIPAL_INFO, array('return_html' => TRUE)
  );

  // Obtain the list of contacts from the chado contact table. 
  $sql = 'SELECT
            contact_id, name 
          FROM {contact} 
          ORDER BY name';
  $results = chado_query($sql);
  $contacts = array();
  while ($contact = $results->fetchObject()) {
    $contacts[$contact->contact_id] = $contact->name;
  }

  $form['choose_contact'] = array(
    '#type' => 'value',
    '#value' => $contacts,
  );

  // *** add a manufacturer cvterm?
  $form['manufacturer_id'] = array(
    '#title' => t('Manufacturer'),
    '#type' => 'select',
    '#empty_value' => '',
    '#required' => TRUE,
    '#description' => t('The manufacturer of the array.'),
    '#default_value' => $manufacturer_id,
    '#options' => $form['choose_contact']['#value'],
    '#suffix' => $lt_message,
  ); 
 
  // Obtain the list of platform types from the cv table arraydesign_platformtype.
  $sql = 'SELECT
            CVT.cvterm_id, CVT.name 
          FROM {cvterm} CVT
            INNER JOIN {cv} CV ON CVT.cv_id = CV.cv_id
          WHERE 
            CV.name = :name 
          ORDER BY CVT.name';
  $results = chado_query($sql, array(':name' => 'arraydesign_platformtype'));
  $platformtypes = array();
  while ($platformtype = $results->fetchObject()) {
    $platformtypes[$platformtype->cvterm_id] = $platformtype->name;
  }
 
  $form['choose_platformtype'] = array(
    '#type' => 'value',
    '#value' => $platformtypes,
  );
 
  // Get the cv_id of arraydesign_platformtype. This is necessary to create the 
  // correct link in the platform form description so the user can create a new 
  // arraydesign_platformtype cv_term.  
  $cv_id = tripal_cv_get_cv(array('name' => 'arraydesign_platformtype'))->cv_id;

  $lt_message = tripal_set_message(t('If the array platform type does not appear in this list, ') 
    . l(t('add a new array platform type'), 'admin/tripal/chado/tripal_cv/cv/' . $cv_id . '/cvterm/add') 
    . t(' to the arraydesign_platformtype controlled vocabulary. A platform type is required.'),
    TRIPAL_INFO, array('return_html' => TRUE)
  );

  $form['platformtype_id'] = array(
    '#title' => t('Platform Type'),
    '#type' => 'select',
    '#empty_value' => '',
    '#required' => TRUE,
    '#description' => t('The array platform.'), 
    '#options' => $form['choose_platformtype']['#value'],
    '#default_value' => $platformtype_id,
    '#suffix' => $lt_message,
  ); 


  // Obtain the list of substrate types from the cv table arraydesign_substratetype.
  $results = chado_query($sql, array(':name' => 'arraydesign_substratetype'));
  $substratetypes = array();
  while ($substratetype = $results->fetchObject()) {
    $substratetypes[$substratetype->cvterm_id] = $substratetype->name;
  }
 
  $form['choose_substratetype'] = array(
    '#type' => 'value',
    '#value' => $substratetypes,
  );

  // Get the cv_id of arraydesign_substratetype. This is necessary to create the 
  // correct link in the substrate form description so the user can create a new 
  // arraydesign_substratetype cv_term.
  $cv_id = tripal_cv_get_cv(array('name' => 'arraydesign_substratetype'))->cv_id;

  $lt_message = tripal_set_message(t('If the array susbstrate type does not appear in the this list, ') 
    . l(t('add a new array substrate type'), 'admin/tripal/chado/tripal_cv/cv/' . $cv_id . '/cvterm/add') 
    . t(' to the arraydesign_substratetype controlled vocabulary. A substrate type is required'),
    TRIPAL_INFO, array('return_html' => TRUE)
  );

  $form['substratetype_id'] = array(
    '#title' => t('Substrate Type'),
    '#type' => 'select',
    '#empty_value' => '',
    '#description' => t('The array substrate.'),
    '#options' => $form['choose_substratetype']['#value'],
    '#default_value' => $substratetype_id,
    '#suffix' => $lt_message,
  ); 

  $lt_message = tripal_set_message(t('If the array design protocol is not present in the dropdown list, ')
    . l(t('add a new protocol.'), 'node/add/chado-protocol') . t(' An array design protocol is not required.'),
    TRIPAL_INFO, array('return_html' => TRUE)
  );

  $arraydesign_protocols = tripal_get_protocol_select_options(TRUE, 'Array Design Protocol');

  $form['protocol_id'] = array(
    '#title' => t('Protocol'),
    '#type' => 'select',
    '#empty_value' => '',
    '#description' => t('The protocol associated with this array design.'),
    '#options' => $arraydesign_protocols, ///$form['choose_protocol']['#value'],
    '#default_value' => $protocol_id,
    '#suffix' => $lt_message,
  );
  
  // Set up the properties form. 
  $prop_cv = tripal_get_default_cv('arraydesignprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL; 
  $details = array(
    'property_table' => 'arraydesignprop', 
    'chado_id' => $arraydesign_id,
    'cv_id' => $cv_id,
  );
  chado_add_node_form_properties($form, $form_state, $details);
  
  return $form;
}

/**
 * Implements hook_validate() for chado_arraydesign.
 * 
 * @ingroup tripal_arraydesign
 */
function chado_arraydesign_validate($node, $form, &$form_state) {
  // Don't validate if a delete.
  if ($node->op == 'Delete') {
    return;
  }

  // We are syncing if there is an organism_id but no node ID. In this case a 
  // validation is not required.  
  if (is_null($node->nid) and property_exists($node, 'arraydesign_id') and $node->arraydesign_id != 0) {
    return;
  } 

  // Remove white space around field values.
  $node->arraydesign_name = trim($node->arraydesign_name);
  $node->version = trim($node->version);
  $node->description = trim($node->description); 
  $node->array_dimensions = trim($node->array_dimensions);
  $node->element_dimensions = trim($node->element_dimensions);
  $node->num_of_elements = trim($node->num_of_elements); 
  $node->num_array_columns = trim($node->num_array_columns);
  $node->num_array_rows = trim($node->num_array_rows);
  $node->num_grid_columns = trim($node->num_grid_columns);
  $node->num_grid_rows = trim($node->num_grid_rows);
  $node->num_sub_columns = trim($node->num_sub_columns);
  $node->num_sub_rows = trim($node->num_sub_rows);

 // Validate for an update.  
  if (property_exists($node, 'arraydesign_id')) {
    $sql = "
      SELECT * 
      FROM {arraydesign} 
      WHERE
        name = :name AND NOT
        arraydesign_id = :arraydesign_id
    "; 
    $args = array(':name' => $node->arraydesign_name, ':arraydesign_id' => $node->arraydesign_id);
    $result = chado_query($sql, $args)->fetchObject(); 
    if ($result) {
      form_set_error('name', t("Update cannot proceed. The array design
        '$node->arraydesign_name' is already present in the database."));
      tripal_report_error('tripal_analysis_expression', TRIPAL_WARNING,
        'Update arraydesign: name already exists: %value', 
        array('%value' => "name = $node->arraydesign_name"));
    }
  }
  // Validate for an insert.
  else {
    $value = array(
      'name' => $node->arraydesign_name,
    );
    $arraydesign = chado_select_record('arraydesign', array('arraydesign_id'), $value);
    if (sizeof($arraydesign) > 0) {
      form_set_error('name', 'Cannot add the array design with this name. 
        The array design already exists.');
      tripal_report_error('tripal_analysis_expression', TRIPAL_WARNING,
        'Insert arraydesign: name already exists: %value',
        array('%value' => "name = $node->arraydesign_name")); 
    }

  }     
  
} 

/**
 * Implements hook_insert(). When the new chado_arraydesign is created we also need to add information 
 * to the chado_arraydesign table. This function is called on insert of a new node
 * of type 'chado_arraydesign' and inserts the necessary information. 
 * 
 * @ingroup tripal_arraydesign
 */
function chado_arraydesign_insert($node) {
  
  $node->arraydesign_name = trim($node->arraydesign_name);
  $node->version = trim($node->version);
  $node->description = trim($node->description); 
  $node->array_dimensions = trim($node->array_dimensions);
  $node->element_dimensions = trim($node->element_dimensions);
  $node->num_of_elements = trim($node->num_of_elements); 
  $node->num_array_columns = trim($node->num_array_columns);
  $node->num_array_rows = trim($node->num_array_rows);
  $node->num_grid_columns = trim($node->num_grid_columns);
  $node->num_grid_rows = trim($node->num_grid_rows);
  $node->num_sub_columns = trim($node->num_sub_columns);
  $node->num_sub_rows = trim($node->num_sub_rows);

  if ($node->protocol_id == "") {
    $protocol_id = NULL;
  } 
  else { 
    $protocol_id = $node->protocol_id; 
  } 

  if ($node->substratetype_id == "") {
    $substratetype_id = NULL;
  } 
  else { 
    $substratetype_id = $node->substratetype_id; 
  } 

  // Check to see if the node is already associated with a chado record. 
  if (!property_exists ($node, 'arraydesign_id')) {
    $values = array(
      'name' => $node->arraydesign_name,
      'version' => $node->version,
      'description' => $node->description,
      'array_dimensions' => $node->array_dimensions,
      'element_dimensions' => $node->element_dimensions,
      'manufacturer_id' => $node->manufacturer_id,
      'protocol_id' => $protocol_id,
      'platformtype_id' => $node->platformtype_id,  
      'substratetype_id' => $substratetype_id, 
    );
    //if ($node->protocol_id == '') {$value['protocol_id'] = NULL;}
    //if ($node->substratetype_id == '') {$value['substratetype_id'] = NULL;}
   /* if ($node->protocol_id != 0) {
      $values['protocol_id'] = $node->protocol_id;
    }
    if ($node->substratetype_id) {
      $values['substratetype_id'] = $node->substratetype_id;
    }*/

    // The chado table expects the following to be integers. The conditional is to prevent    
    // the  integer typecast from typecasting a blank field to '0'.   
    if ($node->num_of_elements) {
      $values['num_of_elements'] = (int)$node->num_of_elements;
    } 
    if ($node->num_array_columns) {
      $values['num_array_columns'] = (int)$node->num_array_columns;
    }
    if ($node->num_array_rows) {
      $values['num_array_rows'] = (int)$node->num_array_rows;
    }
    if ($node->num_grid_columns) {
      $values['num_grid_columns'] = (int)$node->num_grid_columns;
    }
    if ($node->num_grid_rows) {
      $values['num_grid_rows'] = (int)$node->num_grid_rows;
    }
    if ($node->num_sub_columns) {
      $values['num_sub_columns'] = (int)$node->num_sub_columns;
    }
    if ($node->num_sub_rows) {
      $values['num_sub_rows'] = (int)$node->num_sub_rows;
    }

    $arraydesign = chado_insert_record('arraydesign', $values);
    if (!$arraydesign) {
      drupal_set_message(t('Unable to add arraydesign.', 'warning'));
      tripal_report_error('tripal_analysis_expression', TRIPAL_ERROR, 'Insert Array Design: Unable to create arraydesign where values: %values',
      array('%values' => print_r($values, TRUE)));
      return;
    }  
    $arraydesign_id = $arraydesign['arraydesign_id'];

    if ($arraydesign_id) {
      // Set property values. 
      $details = array(
        'property_table' => 'arraydesignprop',
        'base_table' => 'arraydesign', 
        'foreignkey_name' => 'arraydesign_id',
        'foreignkey_value' => $arraydesign_id
      );
      chado_update_node_form_properties($node, $details);    
    } 
  }
  else {
    $arraydesign_id = $node->arraydesign_id;
  }   

  // Check to see if the chado entry has a corresponding chado_arraydesign entry.
  // Add if no corresponding entry exists in the chado_arraydesign table. 
  $check_org_id = chado_get_id_from_nid('arraydesign', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->arraydesign_id = $arraydesign_id;
    drupal_write_record('chado_arraydesign', $record);
  }	
}  

/**
 * Implements hook_load().
 * 
 * When a node is requested by the user this function is called to allow us 
 * to add auxiliary data to the node object.  
 * 
 * @ingroup tripal_arraydesign
 */
function chado_arraydesign_load($nodes) {
  
  foreach ($nodes as $nid => $node) {
    // Find the arraydesign nodes and add details. 
    $arraydesign_id = chado_get_id_from_nid('arraydesign', $nid);
    
    // Ignore orphaned nodes.
    if (!$arraydesign_id) {
      continue;
    }

    // Build the arraydesign variable.
    $values = array('arraydesign_id' => $arraydesign_id);
    $arraydesign = chado_generate_var('arraydesign', $values);
 
    // Add the description field,
    $arraydesign = chado_expand_var($arraydesign, 'field', 'arraydesign.description');
    $nodes[$nid]->arraydesign = $arraydesign;

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
function chado_arraydesign_chado_node_default_title_format() {
  return '[arraydesign.name]';
}

/**
 * Implements hook_chado_node_default_url_format(). 
 *
 * Defines a default URL format for arraydesign nodes. 
 */
function chado_arraydesign_chado_node_default_url_format() {
  return '/arraydesign/[arraydesign.name]';
}

/**
 * Implements hook_node_update().
 * Acts on all content types.
 * 
 * @ingroup tripal_arraydesign
 */
function tripal_arraydesign_node_update($node) {
  
  switch ($node->type) {
    case 'chado_arraydesign':
      
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
function chado_arraydesign_update($node) {

  $node->arraydesign_name = trim($node->arraydesign_name);
  $node->version = trim($node->version);
  $node->description = trim($node->description); 
  $node->array_dimensions = trim($node->array_dimensions);
  $node->element_dimensions = trim($node->element_dimensions);
  $node->num_of_elements = trim($node->num_of_elements); 
  $node->num_array_columns = trim($node->num_array_columns);
  $node->num_array_rows = trim($node->num_array_rows);
  $node->num_grid_columns = trim($node->num_grid_columns);
  $node->num_grid_rows = trim($node->num_grid_rows);
  $node->num_sub_columns = trim($node->num_sub_columns);
  $node->num_sub_rows = trim($node->num_sub_rows);

  $arraydesign_id = chado_get_id_from_nid('arraydesign', $node->nid);
 
  if ($node->revision) {
    // Chado does not do revisions but following the example of prior modules
    // keep this here for when/if revisions are addressed in the future. 
  }
  $match = array(
    'arraydesign_id' => $arraydesign_id,
  );

  if ($node->protocol_id == "") {
  $protocol_id = NULL;
  } else { $protocol_id = $node->protocol_id; } 

  if ($node->substratetype_id == "") {
  $substratetype_id = NULL;
  } else { $substratetype_id = $node->substratetype_id; } 

  $values = array(
    'name' => $node->arraydesign_name,
    'version' => $node->version,
    'description' => $node->description,
    'array_dimensions' => $node->array_dimensions,
    'element_dimensions' => $node->element_dimensions,
    'manufacturer_id' => $node->manufacturer_id,
    'protocol_id' => $protocol_id,
    'platformtype_id' => $node->platformtype_id,  
    'substratetype_id' => $substratetype_id, 
  );

  /*  if ($node->protocol_id != 0) {
      $values['protocol_id'] = $node->protocol_id;
    }
    if ($node->substratetype_id) {
      $values['substratetype_id'] = $node->substratetype_id;
    }*/
 
    // The chado table expects the following to be integers. The conditional is to prevent    
    // the  integer typecast from typecasting a blank field to '0'.   
    if ($node->num_of_elements) {
      $values['num_of_elements'] = (int)$node->num_of_elements;
    } 
    if ($node->num_array_columns) {
      $values['num_array_columns'] = (int)$node->num_array_columns;
    }
    if ($node->num_array_rows) {
      $values['num_array_rows'] = (int)$node->num_array_rows;
    }
    if ($node->num_grid_columns) {
      $values['num_grid_columns'] = (int)$node->num_grid_columns;
    }
    if ($node->num_grid_rows) {
      $values['num_grid_rows'] = (int)$node->num_grid_rows;
    }
    if ($node->num_sub_columns) {
      $values['num_sub_columns'] = (int)$node->num_sub_columns;
    }
    if ($node->num_sub_rows) {
      $values['num_sub_rows'] = (int)$node->num_sub_rows;
    }
  $status = chado_update_record('arraydesign', $match, $values);
  
  if (!$status) {
    drupal_set_message(t('Unable to update arraydesign.'), 'warning');
    tripal_report_error('tripal_analysis_expression', TRIPAL_WARNING, 'Update arraydesign: Unable to update arraydesign where values: %values', array('%values' => print_r($values, TRUE)));
  } 
  
      // Set property values. 
  $details = array(
    'property_table' => 'arraydesignprop',
    'base_table' => 'arraydesign', 
    'foreignkey_name' => 'arraydesign_id',
    'foreignkey_value' => $arraydesign_id
  );
  chado_update_node_form_properties($node, $details);    
  
}

/**
 * Implements hook_delete().
 * 
 * Delete arraydesign from both drupal and chado databases. Check dependencies before
 * deleting from chado. 
 * 
 * @ingroup tripal_arraydesign
 */
function chado_arraydesign_delete($node) {
  $arraydesign_id = chado_get_id_from_nid('arraydesign', $node->nid);
 
  // If there is no arraydesign for this node, this is not the correct type or the chado
  // entry is lost. 
  if (!$arraydesign_id) {
    return;
  }

  // Remove data from drupal tables including chado_arraydesign, node, and node_revisions
  $sql_del = "DELETE FROM {chado_arraydesign} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node_revision} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));

  // Check for dependencies before deleting from the chado database.
  // A dependency is the element table. More dependencies may need to be checked
  // in the future if functionality is extended. 

  $sql = "SELECT element_id FROM {element} WHERE arraydesign_id = :arraydesign_id";
  $check_arraydesign = chado_query($sql, array(':arraydesign_id' => $arraydesign_id))->fetchObject();

  if (!$check_arraydesign) {
    chado_delete_record('arraydesign', array('arraydesign_id' => $arraydesign_id));
  }
  else {
    drupal_set_message(t("Warning: other data depends on this arraydesign. The arraydesign page was removed from this site but the arraydesign record was not removed from Chado."), 'warning'); 
  } 
} 

