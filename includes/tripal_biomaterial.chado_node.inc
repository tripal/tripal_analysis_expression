<?php
/**
 * @file
 * Implements the Protocol content type. 
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
 * @ingroup tripal_biomaterial
 * 
 */
function chado_biomaterial_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }

  if ($node_type == 'chado_biomaterial') {
    if ($op == 'create') {
      if (!user_access('create chado_biomaterial content', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_biomaterial content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_biomaterial content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_biomaterial content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    return NODE_ACCESS_IGNORE;
  }
}

/**
 * Implements hook_form()
 * 
 * When editing or create a new node of type 'chado_biomaterial' we need
 * a form. This function creates the form that will be used for this. 
 * 
 * @ingroup tripal_biomaterial
 */
function chado_biomaterial_form($node, $form_state) {
  $form = array();
 
  // Not sure what this is for. Is this line necessary? 
  $form['#attribute']['enctype'] = 'multipart/form-data';
  
  if (property_exists($node, 'biomaterial')) {   
    $biomaterial = $node->biomaterial;

    // Fill out the form. *** change title in protocl
    $biomaterial_name = property_exists($node, 'biomaterial_name') ? property_exists($node, 'biomaterial_name') : $biomaterial->name;
    $description = property_exists($node, 'description') ? property_exists($node, 'description') : $biomaterial->description;

    // The conditional here is required because this field is chosen by an optional select.
    if ($biomaterial->biosourceprovider_id != NULL) {
      $biosourceprovider_id = property_exists($node, 'biosourceprovider_id') ? property_exists($node, 'biosourceprovider_id') : $biomaterial->biosourceprovider_id->contact_id;
    }
    else {
      $biosourceprovider_id = NULL;
    }

    // The conditional here is required because this field is chosen by an optional select.
    if ($biomaterial->taxon_id != NULL) {
      $taxon_id = property_exists($node, 'taxon_id') ? property_exists($node, 'taxon_id') : $biomaterial->taxon_id->organism_id;
    }
    else {
      $taxon_id = NULL;
    } 
   
    // Set the biomaterial id. 
    $form['biomaterial_id'] = array(
      '#type' => 'value',
      '#value' => $biomaterial->biomaterial_id,
    );
    $biomaterial_id = $biomaterial->biomaterial_id;
  }

  else {
    $biomaterial_name = property_exists($node, 'biomaterial_name') ? property_exists($node, 'biomaterial_name') : '';
    $description = property_exists($node, 'description') ? property_exists($node, 'description') : '';
    $biosourceprovider_id = property_exists($node, 'biosourceprovider_id') ? property_exists($node, 'biosourceprovider_id') : '';
    $taxon_id = property_exists($node, 'taxon_id') ? property_exists($node, 'taxon_id') : '';
    
   $biomaterial_id = NULL;
  }

  $form['biomaterial_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Biomaterial Name (must be unique)'),
    '#required' => TRUE,
    '#default_value' =>  $biomaterial_name,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#rows' => 8,
    '#title' => t('Biomaterial Description'),
    '#default_value' => $description,
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

  $lt_message = tripal_set_message(t('If the contact who provided the biomaterial is not present in the dropdown list, ')
    . l(t('add a new contact.'), 'node/add/chado-contact') . t(' A biomaterial provider contact is not required.'),
    TRIPAL_INFO, array('return_html' => TRUE)
  );

  $form['biosourceprovider_id'] = array(
    '#title' => t('Biomaterial Provider'),
    '#type' => 'select',
    '#empty_value' => '', 
    '#description' => t('The contact who provided the biomaterial.'),
    '#default_value' => $biosourceprovider_id,
    '#options' => $form['choose_contact']['#value'],
    '#suffix' => $lt_message,
  );

  $sql = 'SELECT
            organism_id, genus, species, common_name
          FROM {organism} 
          ORDER BY genus';
  $results = chado_query($sql);
  $organisms = array();
  while ($organism = $results->fetchObject()) {
    $organisms[$organism->organism_id] = $organism->genus . ' ' . $organism->species . ' (' . $organism->common_name . ')';
  }

  $form['choose_organism'] = array(
    '#type' => 'value',
    '#value' => $organisms,
  );

  $lt_message = tripal_set_message(t('If the organism does not appear in this list, ') 
    . l(t('add a new organism.'), 'node/add/chado-organism') . t(' A organism is not required.'),
    TRIPAL_INFO, array('return_html' => TRUE)
  );

  $form['taxon_id'] = array(
    '#title' => t('Organism'),
    '#type' => 'select',
    '#empty_value' => '', 
    '#description' => t('The organism from which the biomaterial was collected.'),
    '#options' => $form['choose_organism']['#value'],
    '#default_value' => $taxon_id,
    '#suffix' => $lt_message,
  );

  // PROPERTIES FORM
  //---------------------------------------------
  $prop_cv = tripal_get_default_cv('biomaterialprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  $details = array(
    'property_table' => 'biomaterialprop',      // the name of the prop table
    'chado_id' => $biomaterial_id,              // the value of feature_id for this record
    'cv_id' => $cv_id                       // the cv.cv_id of the cv governing featureprop.type_id
  );
  chado_add_node_form_properties($form, $form_state, $details);

  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  $details = array(
    'linking_table' => 'biomaterial_dbxref',  // the name of the _dbxref table
    'base_foreign_key' => 'biomaterial_id',   // the name of the key in your base chado table
    'base_key_value' => $biomaterial_id       // the value of feature_id for this record
  );
  chado_add_node_form_dbxrefs($form, $form_state, $details); //*** need to add templates for this add the relationshiops

  // TODO: For some reason adding a relationship to the form breaks AJAX
  // for features (works for other node type)... need to debug

  // RELATIONSHIPS FORM
  //---------------------------------------------
  $relationship_cv = tripal_get_default_cv('biomaterial_relationship', 'type_id');
  $cv_id = $relationship_cv ? $relationship_cv->cv_id : NULL;
  $details = array(
    'relationship_table' => 'biomaterial_relationship',
    'base_table' => 'biomaterial',
    'base_foreign_key' => 'biomaterial_id',
    'base_key_value' => $biomaterial_id,
    'base_name_field' => 'name',
    'nodetype' => 'biomaterial',
    'cv_id' => $cv_id
  );
  chado_add_node_form_relationships($form, $form_state, $details);
 

 
  return $form;
}

/**
 * Implements hook_validate().
 * 
 * @ingroup tripal_biomaterial
 */
function chado_biomaterial_validate($node, $form, &$form_state) {
  // Don't validate if a delete.
  if ($node->op == 'Delete') {
    return;
  }

  // We are syncing if there is an biomaterial_id but no node ID. In this case a 
  // validation is not required.  
  if (is_null($node->nid) and property_exists($node, 'biomaterial_id') and $node->biomaterial_id != 0) {
    return;
  } 

  // Remove white space around field values.
  $node->biomaterial_name = trim($node->biomaterial_name);
  $node->description = trim($node->description);

 // Validate for an update.  
  if (property_exists($node, 'biomaterial_id')) {
    $sql = "
      SELECT * 
      FROM {biomaterial} 
      WHERE
        name = :name AND NOT
        biomaterial_id = :biomaterial_id
    "; 
    $args = array(':name' => $node->biomaterial_name, ':biomaterial_id' => $node->biomaterial_id);
    $result = chado_query($sql, $args)->fetchObject(); 
    if ($result) {
      form_set_error('name', t("Update cannot proceed. The biomaterial
        '$node->biomaterial_name' is already present in the database."));
      tripal_report_error('tripal_analysis_expression', TRIPAL_WARNING,
        'Update biomaterial: name already exists: %value', 
        array('%value' => "name = $node->biomaterial_name"));
    }
  }
  // Validate for an insert.
  else {
    $value = array(
      'name' => $node->biomaterial_name,
    );
    $biomaterial = chado_select_record('biomaterial', array('biomaterial_id'), $value);
    if (sizeof($biomaterial) > 0) {
      form_set_error('biomaterial_title', 'Cannot add the biomaterial with this name. 
        The biomaterial already exists.');
      tripal_report_error('tripal_analysis_expression', TRIPAL_WARNING,
        'Insert biomaterial: name already exists: %value',
        array('%value' => "name = $node->biomaterial_name")); 
    }

  }     
  
}  

/**
 * Implements hook_insert().
 * 
 * When the new chado_biomaterial is created we also need to add information 
 * to the chado_biomaterial table. This function is called on insert of a new node
 * of type 'chado_biomaterial' and inserts the necessary information. 
 * 
 * @ingroup tripal_biomaterial
 */
function chado_biomaterial_insert($node) {
  
  // Remove white space around field values.
  $node->biomaterial_name = trim($node->biomaterial_name);
  $node->description = trim($node->description);

  // Check to see if the node is already associated with a chado record. 
  if (!property_exists ($node, 'biomaterial_id')) {
    $values = array(
      'name' => $node->biomaterial_name,
      'description' => $node->description,
//      'biosourceprovider_id' => $node->biosourceprovider_id,
//      'taxon_id' => $node->taxon_id,
    );

    if ($node->biosourceprovider_id) {
      $values['biosourceprovider_id'] = $node->biosourceprovider_id;
    }

    if ($node->taxon_id) { 
      $values['taxon_id'] = $node->taxon_id;
    }

    $biomaterial = chado_insert_record('biomaterial', $values);
    if (!$biomaterial) {
      drupal_set_message(t('Unable to add biomaterial.'), 'warning');
      tripal_report_error('tripal_analysis_expression', TRIPAL_ERROR, 'Insert Biomaterial: Unable to create biomaterial where values: %values',
      array('%values' => print_r($values, TRUE)));
      return;
    }  
    $biomaterial_id = $biomaterial['biomaterial_id'];
    // * Properties Form *
    $details = array(
      'property_table' => 'biomaterialprop',   // the name of the prop table
      'base_table' => 'biomaterial',           // the name of your chado base table
      'foreignkey_name' => 'biomaterial_id',   // the name of the key in your base table
      'foreignkey_value' => $biomaterial_id    // the value of the biomaterial_id key
    );
    chado_update_node_form_properties($node, $details);

    // * Additional DBxrefs Form *
    $details = array(
      'linking_table' => 'biomaterial_dbxref',   // the name of your _dbxref table
      'foreignkey_name' => 'biomaterial_id',     // the name of the key in your base table
      'foreignkey_value' => $biomaterial_id      // the value of the biomaterial_id key
    );
    chado_update_node_form_dbxrefs($node, $details);
  }
  else {
    $biomaterial_id = $node->biomaterial_id;
  }   

  // Check to see if the chado entry has a corresponding chado_biomaterial entry.
  // Add if no corresponding entry exists in the chado_biomaterial table. 
  $check_org_id = chado_get_id_from_nid('biomaterial', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->biomaterial_id = $biomaterial_id;
    drupal_write_record('chado_biomaterial', $record);
  }	

} 


/**
 * Implements hook_load().
 * 
 *  When a node is requested by the user this function is called to allo us 
 *  to add auxiliary data to the node object.  
 * 
 * @ingroup tripal_biomaterial
 */
function chado_biomaterial_load($nodes) {
  
  foreach ($nodes as $nid => $node) {
    // Find the biomaterial nodes and add details. 
    $biomaterial_id = chado_get_id_from_nid('biomaterial', $nid);
    
    // Ignore orphaned nodes.
    if (!$biomaterial_id) {
      continue;
    }

    // Build the biomaterial variable.
    $values = array('biomaterial_id' => $biomaterial_id);
    $biomaterial = chado_generate_var('biomaterial', $values);
 
    // Add the description field,
    //$arraydesign = chado_expand_var($arraydesign, 'field', 'arraydesign.description');
    $nodes[$nid]->biomaterial = $biomaterial;

    // Get title the node title.
    $node->title = chado_get_node_title($node); 


  } 

} 

/**
 * Implements [content_type]_chado_node_default_title_format().
 * 
 * Defines a default title format for the Chado Node API to set the titles on 
 * chado_arraydesign nodes based on chado fields.
 */ 
function chado_biomaterial_chado_node_default_title_format() {
  return '[biomaterial.name]';
} 

/**
 * Implements hook_chado_node_default_url_format(). 
 *
 * Defines a default URL format for arraydesign nodes. 
 */
function chado_biomaterial_chado_node_default_url_format() {
  return '/biomaterial/[biomaterial.name]';
} 

/**
 * Implements hook_node_insert(). 
 * Acts on all content types. 
 * 
 * @ingroup tripal_biomaterial
 */ 
function tripal_biomaterial_node_insert($node) {
  
  switch ($node->type) {
    case 'chado_biomaterial':
  
      // Find the a biomaterial and add details.     
      $biomaterial_id = chado_get_id_from_nid('biomaterial', $node->nid);
      $values = array('biomaterial_id' => $biomaterial_id);
      $biomaterial = chado_generate_var('biomaterial', $values);
      $node->biomaterial = $biomaterial;
     
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
 * @ingroup tripal_biomaterial
 */
function tripal_biomaterial_node_update($node) {
  
  switch ($node->type) {
    case 'chado_biomaterial':
      
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
 * @ingroup tripal_biomaterial
 */
function chado_biomaterial_update($node) {

  // Remove white space around field values.
  $node->biomaterial_name = trim($node->biomaterial_name);
  $node->description = trim($node->description);

  $biomaterial_id = chado_get_id_from_nid('biomaterial', $node->nid);
 
  if ($node->revision) {
    // Chado does not do revisions but following the example of prior modules
    // keep this here for when/if revisions are addressed in the future. 
  }
  $match = array(
    'biomaterial_id' => $biomaterial_id,
  );

  if ($node->biosourceprovider_id == "") {
    $biosourceprovider_id = NULL;
  }
  else {
    $biosourceprovider_id = $node->biosourceprovider_id;
  }

  if ($node->taxon_id == "") {
    $taxon_id = NULL;
  }
  else {
    $taxon_id = $node->taxon_id;
  }

  $values = array(
    'name' => $node->biomaterial_name,
    'description' => $node->description,
    'biosourceprovider_id' => $biosourceprovider_id,
    'taxon_id' => $taxon_id,
  );

/*    if ($node->biosourceprovider_id) {
      $values['biosourceprovider_id'] = $node->biosourceprovider_id;
    }

    if ($node->taxon_id) { 
      $values['taxon_id'] = $node->taxon_id;
    } */
 
  $status = chado_update_record('biomaterial', $match, $values);

  if (!$status) {
    drupal_set_message(t('Unable to update biomaterial.'), 'warning');
    tripal_report_error('tripal_analysis_expression', TRIPAL_WARNING, 'Update biomaterial: Unable to update biomaterial where values: %values', array('%values' => print_r($values, TRUE)));
  } 

    // * Properties Form *
    $details = array(
      'property_table' => 'biomaterialprop',   // the name of the prop table
      'base_table' => 'biomaterial',           // the name of your chado base table
      'foreignkey_name' => 'biomaterial_id',   // the name of the key in your base table
      'foreignkey_value' => $biomaterial_id    // the value of the biomaterial_id key
    );
    chado_update_node_form_properties($node, $details);

    // * Additional DBxrefs Form *
    $details = array(
      'linking_table' => 'biomaterial_dbxref',   // the name of your _dbxref table
      'foreignkey_name' => 'biomaterial_id',     // the name of the key in your base table
      'foreignkey_value' => $biomaterial_id      // the value of the biomaterial_id key
    );
    chado_update_node_form_dbxrefs($node, $details);

  
} 

/**
 * Implements hook_delete().
 * 
 * Delete biomaterial from both drupal and chado databases. Check dependencies before
 * deleting from chado. 
 * 
 * @ingroup tripal_biomaterial
 */
function chado_biomaterial_delete($node) {
  $biomaterial_id = chado_get_id_from_nid('biomaterial', $node->nid);
 
  // If there is no biomaterial for this node, this is not the correct type or the chado
  // entry is lost. 
  if (!$biomaterial_id) {
    return;
  }

  // Remove data from drupal tables including chado_biomaterial, node, and node_revisions
  $sql_del = "DELETE FROM {chado_biomaterial} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));
  $sql_del = "DELETE FROM {node_revision} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));

  // Check for dependencies before deleting from the chado database.
  // A dependency is the element table. More dependencies may need to be checked
  // in the future if functionality is extended. 

  $check_assay_biomaterial = chado_query('SELECT assay_biomaterial_id FROM {assay_biomaterial} WHERE biomaterial_id = :biomaterial_id', array(':biomaterial_id' => $biomaterial_id))->fetchObject();
  $check_biomaterialprop = chado_query('SELECT biomaterialprop_id FROM {biomaterialprop} WHERE biomaterial_id = :biomaterial_id', array(':biomaterial_id' => $biomaterial_id))->fetchObject();

  if (!$check_assay_biomaterial or !$check_biomaterialprop) {
    chado_delete_record('biomaterial', array('biomaterial_id' => $biomaterial_id));
  }
  else {
    drupal_set_message(t("Warning: other data depends on this biomaterial. The biomaterial page was removed from this site but the biomaterial record was not removed from Chado."), 'warning'); 
  } 
} 

