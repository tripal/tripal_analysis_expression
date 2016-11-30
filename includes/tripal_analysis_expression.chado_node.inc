<?php
/**
 * @file
 * Implements Drupal Node hooks to create the chado_analysis_expression node content type.
 *
 * @ingroup tripal_analysis_expression
 */

/**
 * Implements hook_access().
 *
 * @param $node
 *  The node on which the operation is to be peformed, or, if it does not yet exist, the
 *  type of node to be created.
 *
 * @param $op
 *  The operation to be performed.
 *
 * @param $account
 *  A user object representing the user for whom the operation is to be performed.
 *
 * @return
 *  If the permission for the specified operation is not set then return FALSE. If the
 *  permission is set then return NULL as this allows other modules to disable
 *  access. The only exception is when the $op == 'create'. We will always
 *  return TRUE if the permission is set.
 *
 * @ingroup tirpal_analysis_expression
 */
function chado_analysis_expression_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }

  if ($node_type == 'chado_analysis_expression') {
    if ($op == 'create') {
      if (!user_access('create chado_analysis_expression content', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_analysis_expression content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_analysis_expression content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_analysis_expression content', $account)) {
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
 *  in this module. This module creates four content types: Analysis: Expression,
 *  arraydesign, biomaterial, and protocol.
 *
 * @ingroup tripal_analysis_expression
 */
function tripal_analysis_expression_node_info() {
  $nodes = array();
  $nodes['chado_analysis_expression'] = array(
    'name' => t('Analysis: Expression'),
    'base' => 'chado_analysis_expression',
    'description' => t('An expression analysis using the MAGE module of the chado database.'),
    'has_title' => TRUE,
    'title_label' => t('Analysis: Expression'),
    'locked' => TRUE,
    'chado_node_api' => array(
      'base_table' => 'analysis',
      'hook_prefix' => 'chado_analysis_expression',
      'record_type_title' => array(
        'singular' => t('Analysis: Expression'),
        'plural' => t('Analyses: Expression'),
      ),
      'sync_filters' => array(
        'type_id' => FALSE,
        'organism_id' => FALSE,
        'checkboxes' => array('name'),
      ),
    ),
  );
  $nodes['chado_arraydesign'] = array(
    'name' => t('Array Design'),
    'base' => 'chado_arraydesign',
    'description' => t('An array design from the chado database.'),
    'has_title' => TRUE,
    'locked' => TRUE,
    'chado_node_api' => array(
      'base_table' => 'arraydesign',
      'hook_prefix' => 'chado_arraydesign',
      'record_type_title' => array(
        'singular' => t('Array Design'),
        'plural' => t('Array Designs'),
      ),
      'sync_filters' => array(
        'type_id' => FALSE,
        'organism_id' => FALSE,
        'checkboxes' => array('name'),
      ),
    )
  );
  $nodes['chado_protocol'] = array(
    'name' => t('Protocol'),
    'base' => 'chado_protocol',
    'description' => t('A protocol from the chado database.'),
    'has_title' => TRUE,
    'locked' => TRUE,
    'chado_node_api' => array(
      'base_table' => 'protocol',
      'hook_prefix' => 'chado_protocol',
      'record_type_title' => array(
        'singular' => t('Protocol'),
        'plural' => t('Protocols'),
      ),
      'sync_filters' => array(
        'type_id' => TRUE,
        'organism_id' => FALSE,
        'checkboxes' => array('name'),
      ),
    )
  );
  $nodes['chado_biomaterial'] = array(
    'name' => t('Biomaterial'),
    'base' => 'chado_biomaterial',
    'description' => t('A biomaterial from the chado database.'),
    'has_title' => TRUE,
    'locked' => TRUE,
    'chado_node_api' => array(
      'base_table' => 'biomaterial',
      'hook_prefix' => 'chado_biomaterial',
      'record_type_title' => array(
        'singular' => t('Biomaterial'),
        'plural' => t('Biomaterials'),
      ),
      'sync_filters' => array(
        'type_id' => FALSE,
        'organism_id' => FALSE,
        'checkboxes' => array('name'),
      ),
    )
  );

  return $nodes;
}

/**
 * Implements hook_form().
 *
 * @ingroup tripal_analysis_expression
 */
function chado_analysis_expression_form($node, $form_state) {

  $form = chado_analysis_form($node, $form_state);

  $organism_id = '';
  $associated_analysis_id = '';
  $biosourceprovider_id = '';
  $arraydesign_id = '';
  $assaydate = '';
  $assay_description = '';
  $acquisitiondate = '';
  $acquisition_uri = '';
  $quantificationdate = '';
  $quantification_uri = '';
  $assay_id = '';
  $acquisition_id = '';
  $quantification_id = '';
  $filetype = '';
  $submit_job = '';
  $fileext = '';
  $filepath = '';
  $re_start = '';
  $re_end = '';
  $acquisition_protocol_id = '';
  $quantification_protocol_id = '';
  $quantification_operator_id = '';
  $assay_operator_id = '';
  $assay_protocol_id = '';
  $feature_uniquenames = 'uniq';

  // Load custom fields if present in the database. 
  if (property_exists($node, 'analysis')) {
    $expression = $node->analysis->tripal_analysis_expression;
    $organism_id = $expression->organism_id;
    $associated_analysis_id = $expression->associated_analysis_id;
    $biosourceprovider_id = $expression->biosourceprovider_id;
    $arraydesign_id = $expression->arraydesign_id;
    $assaydate = $expression->assaydate;
    $assay_description = $expression->assay_description;
    $acquisitiondate = $expression->acquisitiondate;
    $acquisition_uri = $expression->acquisition_uri;
    $quantificationdate = $expression->quantificationdate;
    $quantification_uri = $expression->quantification_uri;
    $filetype = $expression->filetype;
    $submit_job = $expression->submit_job;
    $fileext = $expression->fileext;
    $filepath = $expression->filepath;
    $re_start = $expression->re_start;
    $re_end = $expression->re_end;
    $acquisition_protocol_id = $expression->acquisition_protocol_id;
    $quantification_protocol_id = $expression->quantification_protocol_id;
    $quantification_operator_id = $expression->quantification_operator_id;
    $assay_operator_id = $expression->assay_operator_id;
    $assay_protocol_id = $expression->assay_protocol_id;
    $feature_uniquenames = $expression->feature_uniquenames;
  }

  // In the case of a failed validation or an ajax callback, repopulate the fields. 
  if (array_key_exists('values', $form_state)) {
    $organism_id = $form_state['values']['organism_id'];
    $associated_analysis_id = $form_state['values']['associated_analysis_id'];
    $biosourceprovider_id = $form_state['values']['biosourceprovider_id'];
    $arraydesign_id = $form_state['values']['arraydesign_id'];
    $assaydate = $form_state['values']['assaydate'];
    $assay_description = $form_state['values']['description'];
    $acquisitiondate = $form_state['values']['acquisitiondate'];
    $acquisition_uri = $form_state['values']['acquisition_uri'];
    $quantificationdate = $form_state['values']['quantificationdate'];
    $quantification_uri = $form_state['values']['quantification_uri'];
    $filetype = $form_state['values']['filetype'];
    $submit_job = $form_state['values']['submit_job'];
    $fileext = $form_state['values']['fileext'];
    $filepath = $form_state['values']['filepath'];
    $re_start = $form_state['values']['re_start'];
    $re_end = $form_state['values']['re_end'];
    $acquisition_protocol_id = $form_state['values']['acquisition_protocol_id'];
    $quantification_protocol_id = $form_state['values']['quantification_protocol_id'];
    $quantification_operator_id = $form_state['values']['quantification_operator_id'];
    $assay_operator_id = $form_state['values']['assay_operator_id'];
    $assay_protocol_id = $form_state['values']['assay_protocol_id'];
    $feature_uniquenames = $form_state['values']['feature_uniquenames'];
  }

  // Rebuilding form after submission from ajax call (is this necessary?).
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $organism_id = $form_state['input']['organism_id'];
    $associated_analysis_id = $form_state['input']['associated_analysis_id'];
    $biosourceprovider_id = $form_state['input']['biosourceprovider_id'];
    $arraydesign_id = $form_state['input']['arraydesign_id'];
    $assaydate = $form_state['input']['assaydate'];
    $assay_description = $form_state['input']['description'];
    $acquisitiondate = $form_state['input']['acquisitiondate'];
    $acquisition_uri = $form_state['input']['acquisition_uri'];
    $quantificationdate = $form_state['input']['quantificationdate'];
    $quantification_uri = $form_state['input']['quantification_uri'];
    $filetype = $form_state['input']['filetype'];
    $submit_job = $form_state['input']['submit_job'];
    $fileext = $form_state['input']['fileext'];
    $filepath = $form_state['input']['filepath'];
    $re_start = $form_state['input']['re_start'];
    $re_end = $form_state['input']['re_end'];
    $acquisition_protocol_id = $form_state['input']['acquisition_protocol_id'];
    $quantification_protocol_id = $form_state['input']['quantification_protocol_id'];
    $quantification_operator_id = $form_state['input']['quantification_operator_id'];
    $assay_operator_id = $form_state['input']['assay_operator_id'];
    $assay_protocol_id = $form_state['input']['assay_protocol_id'];
    $feature_uniquenames = $form_state['input']['feature_uniquenames'];
  }

  // Set submit_job to 0 every time the form loads.
  $submit_job = 0;

  $form['experimental_design'] = array(
    '#title' => t('Experimental Design'),
    '#type' => 'fieldset',
    '#description' => t('Describe the parameters from which the expression data was generated.'),
    '#attributes' => array('id' => 'expression-extra-settings'),
    '#weight' => 1,
  );

  $form['loader'] = array(
    '#title' => t('Data Loader'),
    '#type' => 'fieldset',
    '#description' => t('Select the expression data to loaded into tripal. The expression data loaders expects the data to be in a flat file.'),
    '#attributes' => array('id' => 'expression-extra-settings'),
    '#weight' => 2,
  );

  $organisms = tripal_get_organism_select_options(TRUE);

  $lt_message = tripal_set_message(t('If the organism is not present in the dropdown list, ')
    . l(t('add a new organism.'), 'node/add/chado-organism') . t(' An organism is not required.'),
    TRIPAL_INFO, array('return_html' => TRUE)
  );

  $form['experimental_design']['organism_id'] = array(
    '#title' => t('Organism'),
    '#type' => 'select',
    '#required' => TRUE,
    '#description' => t('The organism from which the biomaterials were collected.'),
    '#options' => $organisms,
    '#default_value' => $organism_id,
    '#suffix' => $lt_message,
  );

  $analyses = tripal_get_analysis_select_options(FALSE);
  $form['experimental_design']['associated_analysis_id'] = array(
    '#title' => t('Associated Analysis'),
    '#type' => 'select',
    '#description' => t("An analysis associated with the expression analysis. For example, an expression")
      . t(" analysis may be associated with a unigene analysis."),
    '#options' => $analyses,
    '#default_value' => $associated_analysis_id,
  );

  // At this time the analysis module has the extra fields.
  // These fields do not appear to do anything for the analysis module.
  // The following three form fields prevent an error form appearing 
  // when the expression biomaterial is submitted. 
  $form['removed'] = array(
    '#type' => 'value',
    '#default_value' => '',
  );
  $form['num_new'] = array(
    '#type' => 'value',
    '#default_value' => '',
  );
  $form['analysis_type'] = array(
    '#type' => 'value',
    '#default_value' => '',
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

  $lt_message = tripal_set_message(t('If the biomaterial provider is not present in the dropdown list, ')
    . l(t('add a new contact.'), 'node/add/chado-contact') . t(' A biomaterial provider is not required.'),
    TRIPAL_INFO, array('return_html' => TRUE)
  );

  $form['experimental_design']['biosourceprovider_id'] = array(
    '#title' => t('Biomaterial Provider'),
    '#type' => 'select',
    '#empty_value' => '',
    '#description' => t('The contact who provided the biomaterial. If the biomaterial is already')
      . (' present in the database, the biomaterial provider selected here will')
      . (' overwrite the biomaterial provider set for the biomaterial in the database.'),
    '#default_value' => $biosourceprovider_id,
    '#options' => $form['choose_contact']['#value'],
    '#suffix' => $lt_message,
  );

  $lt_message = tripal_set_message(t('If the array design does not appear in this list, ')
    . l(t('add a new arraydesign.'), 'node/add/chado-arraydesign')
    . t(' If an array design is not selected, then a generic array design will be created for the analysis.'),
    TRIPAL_INFO, array('return_html' => TRUE)
  );

  $arraydesigns = tripal_get_arraydesign_select_options(TRUE);

  $form['experimental_design']['arraydesign_id'] = array(
    '#title' => t('Array Design'),
    '#type' => 'select',
    '#description' => t('The array design associated with this analysis. This is not required if the experimental')
      . t(' data was gathered from next generation sequencing methods. If an array design is not')
      . t(' selected, a generic array design will be used as a place filler.'),
    '#options' => $arraydesigns,
    '#default_value' => $arraydesign_id,
    '#suffix' => $lt_message,
  );

  $form['experimental_design']['acquisition_details'] = array(
    '#title' => t('Acquisition Details'),
    '#type' => 'fieldset',
    '#description' => t('Describe the details of the acquisition process. The acquisition is the process')
      . (' of scanning the experimental data.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 1,
  );

  // Get time saved in chado.
  $default_time = $acquisitiondate;
  $year = preg_replace("/^(\d+)-\d+-\d+ .*/", "$1", $default_time);
  $month = preg_replace("/^\d+-0?(\d+)-\d+ .*/", "$1", $default_time);
  $day = preg_replace("/^\d+-\d+-0?(\d+) .*/", "$1", $default_time);

  // If the time is not set, use current time.
  if (!$default_time) {
    $default_time = REQUEST_TIME;
    $year = format_date($default_time, 'custom', 'Y');
    $month = format_date($default_time, 'custom', 'n');
    $day = format_date($default_time, 'custom', 'j');
  }

  $form['experimental_design']['acquisition_details']['acquisitiondate'] = array(
    '#type' => 'date',
    '#title' => t('Date Acquisition Run'),
    '#description' => t('If no date is provided, then the current date will be used.'),
    '#required' => TRUE,
    '#default_value' => array(
      'year' => $year,
      'month' => $month,
      'day' => $day,
    ),
  );

  $form['experimental_design']['acquisition_details']['acquisition_uri'] = array(
    '#type' => 'textfield',
    '#title' => t('Acquisition URI'),
    '#description' => t('URI location that describes the acquisition.'),
    '#default_value' => $acquisition_uri,
  );

  $lt_message = tripal_set_message(t('If the acquisition protocol is not present in the dropdown list, ')
    . l(t('add a new protocol.'), 'node/add/chado-protocol') . t(' An acquisition protocol is not required.'),
    TRIPAL_INFO, array('return_html' => TRUE)
  );

  $acquisitions = tripal_get_protocol_select_options(TRUE, 'Acquisition Protocol');

  $form['experimental_design']['acquisition_details']['acquisition_protocol_id'] = array(
    '#title' => t('Acquisition Protocol'),
    '#type' => 'select',
    '#description' => t('The protocol associated with this acquisition.'),
    '#options' => $acquisitions,
    '#default_value' => $acquisition_protocol_id,
    '#suffix' => $lt_message,
  );

  $form['experimental_design']['quantification_details'] = array(
    '#title' => t('Quantification Details'),
    '#type' => 'fieldset',
    '#description' => t('Describe the details of the quantification process. The quantification is the process of transforming raw expression data into numeric data.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 1,
  );

  // Get time saved in chado
  $default_time = $quantificationdate;
  $year = preg_replace("/^(\d+)-\d+-\d+ .*/", "$1", $default_time);
  $month = preg_replace("/^\d+-0?(\d+)-\d+ .*/", "$1", $default_time);
  $day = preg_replace("/^\d+-\d+-0?(\d+) .*/", "$1", $default_time);
  // If the time is not set, use current time
  if (!$default_time) {
    $default_time = REQUEST_TIME;
    $year = format_date($default_time, 'custom', 'Y');
    $month = format_date($default_time, 'custom', 'n');
    $day = format_date($default_time, 'custom', 'j');
  }
  $form['experimental_design']['quantification_details']['quantificationdate'] = array(
    '#type' => 'date',
    '#title' => t('Date Quantification Run'),
    '#description' => t('If no date is provided, then the current date will be used.'),
    '#required' => TRUE,
    '#default_value' => array(
      'year' => $year,
      'month' => $month,
      'day' => $day,
    ),
  );

  $form['experimental_design']['quantification_details']['quantification_uri'] = array(
    '#type' => 'textfield',
    '#title' => t('Quantification URI'),
    '#description' => t('URI location that describes the quantification.'),
    '#default_value' => $quantification_uri,
  );

  $lt_message = tripal_set_message(t('If the quantification operator is not present in the dropdown list, ')
    . l(t('add a new contact.'), 'node/add/chado-contact') . t('A quantication operator is not required.'),
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

  $form['experimental_design']['quantification_details']['quantification_operator_id'] = array(
    '#title' => t('Quantification Operator'),
    '#type' => 'select',
    '#empty_value' => '',
    '#description' => t('The quantification operator. If the operator does not appear in this list, ') . l(t('add a new contact.'), 'node/add/chado-contact') . t(' An operator is not required.'),
    '#default_value' => $quantification_operator_id,
    //*** make sure this works -names - contact/operator
    '#options' => $form['choose_contact']['#value'],
    '#suffix' => $lt_message,
  );

  $lt_message = tripal_set_message(t('If the quantification protocol is not present in the dropdown list, ')
    . l(t('add a new protocol.'), 'node/add/chado-protocol') . t(' A quantification protocol is not required.'),
    TRIPAL_INFO, array('return_html' => TRUE)
  );

  $quantifications = tripal_get_protocol_select_options(TRUE, 'Quantification Protocol');

  $form['experimental_design']['quantification_details']['quantification_protocol_id'] = array(
    '#title' => t('Quantification Protocol'),
    '#type' => 'select',
    '#description' => t('The protocol associated with this quantification.'),
    '#options' => $quantifications,
    '#default_value' => $quantification_protocol_id,
    '#suffix' => $lt_message,
  );

  $form['loader']['filetype'] = array(
    '#type' => 'radios',
    '#title' => t('Source File Type'),
    '#description' => t('Data can be loaded from two format types. Select the column format for files that have two columns - transcript id and expression value. Select the matrix format for files that specify biosample by column and transcript by row. If "Column Format" is selected, the name of the column file will be taken as the biomaterial name. It is recommended to avoid the use of white space in column file names and in biomaterial names.'),
    '#options' => array(
      'col' => t('Column Format'),
      'mat' => t('Matrix Format'),
    ),
    '#default_value' => $filetype,
  );

  $form['loader']['submit_job'] = array(
    '#type' => 'checkbox',
    '#title' => t('Submit a job to parse the expression data into Chado'),
    '#description' => t('Note: Features associated with the expression data must exist in Chado before loading the file. Otherwise, the loader will exit with an error.'),
    '#default_value' => 0,
  );

  $form['loader']['feature_uniquenames'] = array(
    '#type' => 'radios',
    '#title' => t('Name Match Type'),
    '#description' => t('Expression data can be associated with features via the feature name or the feature unique name.'),
    '#options' => array(
      'name' => t('Name'),
      'uniq' => t('Unique name'),
    ),
    '#default_value' => $feature_uniquenames,
  );

  /*  $form['loader']['feature_uniquenames'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use feature uniquename to associate expression values (default is feature name)'),
      '#default_value' => $feature_uniquenames,
    );*/

  $form['loader']['fileext'] = array(
    '#type' => 'textfield',
    '#title' => t('File Type Suffix'),
    '#description' => t('Select the file extension for the file(s) to be loaded into Chado. Do not include the ".". It is not necessary to specifiy a file extension for a matrix file.'),
    '#default_value' => $fileext,
  );

  $form['loader']['filepath'] = array(
    '#type' => 'textfield',
    '#title' => t('File Path'),
    '#description' => t('Select the path to the expression file(s). If the file path is a directory, then the loader will load every file of the type specified in "File Type Suffix" from the directory.'),
    '#default_value' => $filepath,
  );

  $form['loader']['re_start'] = array(
    '#type' => 'textfield',
    '#title' => t('Regex for Start of Data'),
    '#description' => t('If the expression file to be loaded has a header text, use a regular expression to describe the line that occurs before the start of the expression data. If the file has no header text, this line may be left blank.'),
    '#default_value' => $re_start,
  );

  $form['loader']['re_end'] = array(
    '#type' => 'textfield',
    '#title' => t('Regex for End of Data'),
    '#description' => t('If the expression file to be loaded has a footer text, use a regular expression to describe the line that occurs after the end of the expression data. If the file has no footer text, this line may be left blank.'),
    '#default_value' => $re_end,
  );

  $lt_message = tripal_set_message(t('Expression values are associated with their respective biomaterial based on the name of the biomaterial. Running this check will show whether the biomaterial names in the expression file have a corresponding biomaterial in the database. This can be useful when biomaterials have been loaded using the biomaterial loader. If a biomaterial is not present in the database then the expression loader will create a biomaterial.'), TRIPAL_INFO, array('return_html' => TRUE));

  $form['loader']['biomaterial_validator'] = array(
    '#type' => 'submit',
    '#value' => t('Check Biomaterials'),
    '#suffix' => $lt_message,
    '#submit' => array('chado_analysis_expression_biomaterial_validator_submit'),
  );

  $form['experimental_design']['assay_details'] = array(
    '#title' => t('Assay Details'),
    '#type' => 'fieldset',
    '#description' => t('Describe the details of the assay process. The assay is the physical instance of the array')
      . t(' used in the experiment.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 1,
  );

  // Get time saved in chado
  $default_time = $assaydate;
  $year = preg_replace("/^(\d+)-\d+-\d+ .*/", "$1", $default_time);
  $month = preg_replace("/^\d+-0?(\d+)-\d+ .*/", "$1", $default_time);
  $day = preg_replace("/^\d+-\d+-0?(\d+) .*/", "$1", $default_time);
  // If the time is not set, use current time
  if (!$default_time) {
    $default_time = REQUEST_TIME;
    $year = format_date($default_time, 'custom', 'Y');
    $month = format_date($default_time, 'custom', 'n');
    $day = format_date($default_time, 'custom', 'j');
  }
  $form['experimental_design']['assay_details']['assaydate'] = array(
    '#type' => 'date',
    '#title' => t('Date Assay Run'),
    '#description' => t('If no date is provided, then the current date will be used.'),
    '#required' => TRUE,
    '#default_value' => array(
      'year' => $year,
      'month' => $month,
      'day' => $day,
    ),
  );

  $form['experimental_design']['assay_details']['assay_description'] = array(
    '#type' => 'textfield',
    '#title' => t('Assay Description'),
    '#description' => t('A description of the assay used.'),
    '#default_value' => $assay_description,
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

  $lt_message = tripal_set_message(t('If the assay operator is not present in the dropdown list, ')
    . l(t('add a new contact.'), 'node/add/chado-contact') . t(' If an assay operator is not selected, then a generic assay operator will be created for this analysis.'),
    TRIPAL_INFO, array('return_html' => TRUE)
  );

  $form['experimental_design']['assay_details']['assay_operator_id'] = array(
    '#title' => t('Assay Operator'),
    '#type' => 'select',
    '#empty_value' => '',
    '#description' => t('The assay operator.'),
    '#default_value' => $assay_operator_id,
    //*** make sure this works -names - contact/operator
    '#options' => $form['choose_contact']['#value'],
    '#suffix' => $lt_message,
  );

  $lt_message = tripal_set_message(t('If the assay protocol is not present in the dropdown list, ')
    . l(t('add a new protocol.'), 'node/add/chado-protocol') . t(' An assay protocol is not required.'),
    TRIPAL_INFO, array('return_html' => TRUE)
  );

  $assays = tripal_get_protocol_select_options(TRUE, 'Assay Protocol');

  $form['experimental_design']['assay_details']['assay_protocol_id'] = array(
    '#title' => t('Assay Protocol'),
    '#type' => 'select',
    '#description' => t('The protocol associated with this assay.'),
    '#options' => $assays,
    '#default_value' => $assay_protocol_id,
    '#suffix' => $lt_message,
  );

  return $form;
}


function chado_analysis_expression_biomaterial_validator_submit($form, &$form_state) {

  $filepath = $form_state['values']['filepath'];
  $filetype = $form_state['values']['filetype'];
  $fileext = $form_state['values']['fileext'];
  $re_start = $form_state['values']['re_start'];
  $re_end = $form_state['values']['re_end'];
  $first_mes = 0;

  // Make sure the form rebuilds.
  $form_state['rebuild'] = TRUE;

///
  $re_ext = '/^.+\.(' . $fileext . ')/';

  // Check if file is readable or issue a warning
  if(!is_readable($filepath)) {
    drupal_set_message(t("Error: the file \"$filepath\" is not readable. Please check its permissions."));
  }

  // A matrix file.
  if ($filetype == 'mat' and is_file($filepath)) {
    $data_ln = 0;

    $mat_fp = fopen($filepath, 'r');
    //Check a single matrix file.
    while ($line = fgets($mat_fp)) {
      if ($data_ln == 1 or !$re_start) {
        // Make sure this is actually a tab separated file.
        if (!preg_match('/\t/', $line, $matches)) {
          drupal_set_message(t('Warning: the expression loader expects a tab separated file. Please ensure the expression file is a tab separated file. Warning thrown on line:') . "\n", 'warning');
          drupal_set_message($line, 'warning');
          return;
        }
        $linepart = preg_split('/\t/', $line, -1, PREG_SPLIT_NO_EMPTY);
        // Trim whitespace from biomaterial names.
        foreach ($linepart as $biomaterial) {
          $new_linepart[] = trim($biomaterial);
        }
        $linepart = $new_linepart;
        $num_col = count($linepart);
        foreach ($linepart as $sample) {
          if (chado_select_record('biomaterial', array('biomaterial_id'), array('name' => $sample), array('has_record' => TRUE)) < 1) {
            if ($first_mes == 0) {
              drupal_set_message("The following biosamples from the expression file do not have a corresponding biomaterial record in the Chado database. If you have already loaded biomaterial records, please resolve name differences prior to submitting the expression loading job. When the loading job is submitted a new biomaterial will be created for biomaterials from the expression file without a name match in the Chado database.");
              $first_mes = 1;
            }
            drupal_set_message($sample);
          }
        }
        break;
      }
      if (preg_match('/' . $re_start . '/', $line) and $re_start) {
        $data_ln = 1;
      }
    }
    if ($first_mes == 0) {
      drupal_set_message("All biomaterials found in the expression matrix file have a corresponding biomaterial record in the Chado database.");
    }
  }

  // The column format.
  elseif ($filetype == 'col' and is_file($filepath) and preg_match("/^.+\." . $fileext . "$/", $filepath)) {
    // Check single column file.
    preg_match('/^.+\/(.+)\.' . $fileext . '$/', $filepath, $biomaterial);
    if (chado_select_record('biomaterial', array('biomaterial_id'), array('name' => $biomaterial[1]), array('has_record' => TRUE)) < 1) {
      drupal_set_message("The following biomaterial from the expression file does not have a corresponding biomaterial record in the Chado database. If you have already loaded biomaterial records, please resolve name differences prior to submitting the expression loading job. When the loader job is submitted a new biomaterial will be created for the biomaterial from the expression file without a name match in the Chado database.");
      $first_mes = 1;
      drupal_set_message($biomaterial[1]);
    }
    if ($first_mes == 0) {
      drupal_set_message("The biomaterial found in the file has a corresponding biomaterial record in the Chado database.");
    }

  }
  else { // Multiple files in the column format.
    $dir_handle = @opendir($filepath);
    if (!$dir_handle) {
      return;
    }
    $total_files = count(glob($filepath . '/*.' . $fileext));
    $num_file = 0;

    // Load all files with of type $fileext in the directory.
    while ($file = readdir($dir_handle)) {
      if (preg_match('/(.+)\.' . $fileext . '$/', $file, $biomaterial)) {
        // Check multiple column files.
        if (chado_select_record('biomaterial', array('biomaterial_id'), array('name' => $biomaterial[1]), array('has_record' => TRUE)) < 1) {
          if ($first_mes == 0) {
            drupal_set_message("The following biosamples from the expression file do not have a corresponding biomaterial record in the Chado database. If you have already loaded biomaterial records, please resolve name differences prior to submitting the expression loading job. When the loading job is submitted a new biomaterial will be created for biomaterials from the expression file without a name match in the Chado database.");
            $first_mes = 1;
          }
          drupal_set_message($biomaterial[1]);
        }
        $num_file++;
      }
    }
    if ($first_mes == 0) {
      drupal_set_message("All biomaterials found in the directory have a corresponding biomaterial record in the Chado database.");
    }
  }
///
}


/**
 * Implementation of hook_validate.
 *
 * This function validates the chado_analysis_expression form.
 *
 * @ingroup tripal_analysis_expression
 */
function chado_analysis_expression_validate($node, $form, &$form_state) {
  // Don't validate if a delete.
  if ($node->op == 'Delete') {
    return;
  }

  // We are syncing if there is an content_type_id but no node ID. In this case a 
  // validation is not required.  
  if (is_null($node->nid) and property_exists($node, 'content_type_id') and $node->content_type_id != 0) {
    return;
  }

  // Check for an organism.
  if (!$node->organism_id) {
    form_set_error('organism_id', 'Please select the organism from which the biomaterials were collected.');
  }

  // Remove white space around field values.
  $node->fileext = trim($node->fileext);
  $node->filepath = trim($node->filepath);
  $node->re_start = trim($node->re_start);
  $node->re_end = trim($node->re_end);

  // Validate the regular expressions if provided.
  if ($node->re_start and @preg_match('/' . $node->re_start . '/', NULL) === FALSE) {
    form_set_error('re_start', 'Invalid regular expression for the start of expression data: ' . $node->re_start);
  }
  if ($node->re_end and @preg_match('/' . $node->re_end . '/', NULL) === FALSE) {
    form_set_error('re_end', 'Invalid regular expression for the end of expression data: ' . $node->re_end);
  }

  // Make sure a file type is selected if a file path is provided.
  if ($node->filepath and !$node->filetype) {
    form_set_error('filetype', 'Please select a source file type.');
  }

  if (!is_file($node->filepath) and !is_dir($node->filepath)) {
    form_set_error('filepath', 'Please ensure that the file or directory in the file path is present and/or readable');
  }

  // Check to see if the matrix file is readable.
  if ($node->filepath and $node->filetype === 'mat' and (!is_file($node->filepath) or !is_readable($node->filepath)) and !is_dir($node->filepath)) {
    form_set_error('filepath', 'Unable to read expression file: ' . $node->filepath);
  }
  elseif ($node->filepath and $node->filetype === 'mat' and is_dir($node->filepath)) {
    form_set_error('filepath', 'When loading an expression file using the matrix format please provide a path to a single file. The file path must include the file name.');
  }
  elseif ($node->filepath and $node->filetype === 'col' and is_file($node->filepath) and !is_readable($node->filepath)) {
    form_set_error('filepath', 'Unable to read expression file: ' . $node->filepath);
  }
  elseif ($node->filepath and $node->filetype === 'col' and is_file($node->filepath) and !$node->fileext) {
    form_set_error('fileext', 'Please specify a file extension for the column file to be loaded.');
  }
  elseif ($node->filepath and $node->filetype === 'col' and is_dir($node->filepath)) {
    if (empty($node->fileext)) {
      form_set_error('fileext', 'Please specify a file extension for the column files to be loaded.');
    }
    $anyfile = 0;
    $dir_handle = @opendir($node->filepath);
    if (!$dir_handle) {
      form_set_error('filepath', 'Unable to open the directory: ' . $node->filepath);
    }
    while ($file = readdir($dir_handle)) {
      if (preg_match('/(.+)\.' . $node->fileext . '$/', $file)) {
        $anyfile = 1;
      }
      if (preg_match('/(.+)\.' . $node->fileext . '$/', $file) and !is_readable($node->filepath . '/' . $file)) {
        form_set_error('filepath', 'At least one file of type: ' . $node->fileext . ' is unreadable. Error caught when attempting to read file: ' . $node->filepath . '/' . $file);
      }
    }
    if ($anyfile == 0) {
      form_set_error('filepath', 'No files of type: ' . $node->fileext . ' are found in the directory: ' . $node->filepath);
    }
  }
  //elseif ($node->filepath and $node->filetype === 'col' and !is_dir($node->filepath)) {
  // form_set_error('filepath', 'Unable to open the directory: ' . $node->filepath . '. Please ensure that the file type suffix is correctly set and that the server has permission to access the file(s) or directory.');
  // }
}

/**
 * Implements hook_insert().
 *
 * This function inserts the data from the analysis: expression form into the chado database.
 * The analysis: expression form also allows the user to submit an expression loading job.
 *
 * @ingroup tripal_analysis_expression
 */
function chado_analysis_expression_insert($node) {

  // Hook into the tripal analysis insert.
  chado_analysis_insert($node);

  // Remove white space around field values.
  $node->fileext = trim($node->fileext);
  $node->filepath = trim($node->filepath);
  $node->re_start = trim($node->re_start);
  $node->re_end = trim($node->re_end);

  $assaydate = $node->assaydate['year'] . "-" . $node->assaydate['month'] . "-" . $node->assaydate['day'] . " 00:00:00";
  $acquisitiondate = $node->acquisitiondate['year'] . "-" . $node->acquisitiondate['month'] . "-" . $node->acquisitiondate['day'] . " 00:00:00";
  $quantificationdate = $node->quantificationdate['year'] . "-" . $node->quantificationdate['month'] . "-" . $node->quantificationdate['day'] . " 00:00:00";

  $record = array('table' => 'analysis', 'id' => $node->analysis_id);

  // Add analysis: expression fields. 
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_organism_id',
    'cv_name' => 'tripal',
    'value' => trim($node->organism_id)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_associated_analysis_id',
    'cv_name' => 'tripal',
    'value' => trim($node->associated_analysis_id)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_biosourceprovider_id',
    'cv_name' => 'tripal',
    'value' => trim($node->biosourceprovider_id)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_arraydesign_id',
    'cv_name' => 'tripal',
    'value' => trim($node->arraydesign_id)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_assaydate',
    'cv_name' => 'tripal',
    'value' => $assaydate
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_assay_description',
    'cv_name' => 'tripal',
    'value' => trim($node->assay_description)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_acquisitiondate',
    'cv_name' => 'tripal',
    'value' => $acquisitiondate
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_acquisition_uri',
    'cv_name' => 'tripal',
    'value' => trim($node->acquisition_uri)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_quantificationdate',
    'cv_name' => 'tripal',
    'value' => $quantificationdate
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_quantification_uri',
    'cv_name' => 'tripal',
    'value' => trim($node->quantification_uri)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_assay_protocol_id',
    'cv_name' => 'tripal',
    'value' => trim($node->assay_protocol_id)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_acquisition_protocol_id',
    'cv_name' => 'tripal',
    'value' => trim($node->acquisition_protocol_id)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_quantification_protocol_id',
    'cv_name' => 'tripal',
    'value' => trim($node->quantification_protocol_id)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_assay_operator_id',
    'cv_name' => 'tripal',
    'value' => trim($node->assay_operator_id)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_quantification_operator_id',
    'cv_name' => 'tripal',
    'value' => trim($node->quantification_operator_id)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_filetype',
    'cv_name' => 'tripal',
    'value' => trim($node->filetype)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_submit_job',
    'cv_name' => 'tripal',
    'value' => trim($node->submit_job)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_fileext',
    'cv_name' => 'tripal',
    'value' => trim($node->fileext)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_filepath',
    'cv_name' => 'tripal',
    'value' => trim($node->filepath)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_re_start',
    'cv_name' => 'tripal',
    'value' => trim($node->re_start)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_re_end',
    'cv_name' => 'tripal',
    'value' => trim($node->re_end)
  ));
  chado_insert_property($record, array(
    'type_name' => 'analysis_expression_feature_uniquenames',
    'cv_name' => 'tripal',
    'value' => trim($node->feature_uniquenames)
  ));

  if ($node->submit_job) {
    // Submit the expression loading job.
    chado_analysis_expression_submit_job($node);
  }
}

/**
 * Implements hook_load().
 *
 * This function loads the chado_analysis_expression content type.
 *
 * @ingroup tripal_analysis_expression
 */
function chado_analysis_expression_load($nodes) {

  // Load the default analysis fields. 
  chado_analysis_load($nodes);

  foreach ($nodes as $nid => $node) {

    // Create some variables for easier lookup.
    $analysis = $node->analysis;
    $analysis_id = $analysis->analysis_id;
    $record = array('table' => 'analysis', 'id' => $analysis->analysis_id);

    // Load the analysis expression property fields. 
    $organism_id = chado_get_property($record, array(
      'type_name' => 'analysis_expression_organism_id',
      'cv_name' => 'tripal'
    ));
    $associated_analysis_id = chado_get_property($record, array(
      'type_name' => 'analysis_expression_associated_analysis_id',
      'cv_name' => 'tripal'
    ));
    $biosourceprovider_id = chado_get_property($record, array(
      'type_name' => 'analysis_expression_biosourceprovider_id',
      'cv_name' => 'tripal'
    ));
    $arraydesign_id = chado_get_property($record, array(
      'type_name' => 'analysis_expression_arraydesign_id',
      'cv_name' => 'tripal'
    ));
    $assaydate = chado_get_property($record, array(
      'type_name' => 'analysis_expression_assaydate',
      'cv_name' => 'tripal'
    ));
    $assay_description = chado_get_property($record, array(
      'type_name' => 'analysis_expression_assay_description',
      'cv_name' => 'tripal'
    ));
    $acquisitiondate = chado_get_property($record, array(
      'type_name' => 'analysis_expression_acquisitiondate',
      'cv_name' => 'tripal'
    ));
    $acquisition_uri = chado_get_property($record, array(
      'type_name' => 'analysis_expression_acquisition_uri',
      'cv_name' => 'tripal'
    ));
    $quantificationdate = chado_get_property($record, array(
      'type_name' => 'analysis_expression_quantificationdate',
      'cv_name' => 'tripal'
    ));
    $quantification_uri = chado_get_property($record, array(
      'type_name' => 'analysis_expression_quantification_uri',
      'cv_name' => 'tripal'
    ));
    $assay_protocol_id = chado_get_property($record, array(
      'type_name' => 'analysis_expression_assay_protocol_id',
      'cv_name' => 'tripal'
    ));
    $acquisition_protocol_id = chado_get_property($record, array(
      'type_name' => 'analysis_expression_acquisition_protocol_id',
      'cv_name' => 'tripal'
    ));
    $quantification_protocol_id = chado_get_property($record, array(
      'type_name' => 'analysis_expression_quantification_protocol_id',
      'cv_name' => 'tripal'
    ));
    $assay_operator_id = chado_get_property($record, array(
      'type_name' => 'analysis_expression_assay_operator_id',
      'cv_name' => 'tripal'
    ));
    $quantification_operator_id = chado_get_property($record, array(
      'type_name' => 'analysis_expression_quantification_operator_id',
      'cv_name' => 'tripal'
    ));
    $filetype = chado_get_property($record, array(
      'type_name' => 'analysis_expression_filetype',
      'cv_name' => 'tripal'
    ));
    $submit_job = chado_get_property($record, array(
      'type_name' => 'analysis_expression_submit_job',
      'cv_name' => 'tripal'
    ));
    $fileext = chado_get_property($record, array(
      'type_name' => 'analysis_expression_fileext',
      'cv_name' => 'tripal'
    ));
    $filepath = chado_get_property($record, array(
      'type_name' => 'analysis_expression_filepath',
      'cv_name' => 'tripal'
    ));
    $re_start = chado_get_property($record, array(
      'type_name' => 'analysis_expression_re_start',
      'cv_name' => 'tripal'
    ));
    $re_end = chado_get_property($record, array(
      'type_name' => 'analysis_expression_re_end',
      'cv_name' => 'tripal'
    ));
    $feature_uniquenames = chado_get_property($record, array(
      'type_name' => 'analysis_expression_feature_uniquenames',
      'cv_name' => 'tripal'
    ));

    $analysis->tripal_analysis_expression = new stdClass;
    $analysis->tripal_analysis_expression->organism_id = is_object($organism_id) ? $organism_id->value : '';
    $analysis->tripal_analysis_expression->associated_analysis_id = is_object($associated_analysis_id) ? $associated_analysis_id->value : '';
    $analysis->tripal_analysis_expression->biosourceprovider_id = is_object($biosourceprovider_id) ? $biosourceprovider_id->value : '';
    $analysis->tripal_analysis_expression->arraydesign_id = is_object($arraydesign_id) ? $arraydesign_id->value : '';
    $analysis->tripal_analysis_expression->assaydate = is_object($assaydate) ? $assaydate->value : '';
    $analysis->tripal_analysis_expression->assay_description = is_object($assay_description) ? $assay_description->value : '';
    $analysis->tripal_analysis_expression->acquisitiondate = is_object($acquisitiondate) ? $acquisitiondate->value : '';
    $analysis->tripal_analysis_expression->acquisition_uri = is_object($acquisition_uri) ? $acquisition_uri->value : '';
    $analysis->tripal_analysis_expression->quantificationdate = is_object($quantificationdate) ? $quantificationdate->value : '';
    $analysis->tripal_analysis_expression->quantification_uri = is_object($quantification_uri) ? $quantification_uri->value : '';
    $analysis->tripal_analysis_expression->assay_protocol_id = is_object($assay_protocol_id) ? $assay_protocol_id->value : '';
    $analysis->tripal_analysis_expression->acquisition_protocol_id = is_object($acquisition_protocol_id) ? $acquisition_protocol_id->value : '';
    $analysis->tripal_analysis_expression->quantification_protocol_id = is_object($quantification_protocol_id) ? $quantification_protocol_id->value : '';
    $analysis->tripal_analysis_expression->assay_operator_id = is_object($assay_operator_id) ? $assay_operator_id->value : '';
    $analysis->tripal_analysis_expression->quantification_operator_id = is_object($quantification_operator_id) ? $quantification_operator_id->value : '';
    $analysis->tripal_analysis_expression->filetype = is_object($filetype) ? $filetype->value : '';
    $analysis->tripal_analysis_expression->submit_job = is_object($submit_job) ? $submit_job->value : '';
    $analysis->tripal_analysis_expression->fileext = is_object($fileext) ? $fileext->value : '';
    $analysis->tripal_analysis_expression->filepath = is_object($filepath) ? $filepath->value : '';
    $analysis->tripal_analysis_expression->re_start = is_object($re_start) ? $re_start->value : '';
    $analysis->tripal_analysis_expression->re_end = is_object($re_end) ? $re_end->value : '';
    $analysis->tripal_analysis_expression->feature_uniquenames = is_object($feature_uniquenames) ? $feature_uniquenames->value : '';

    // Acquire the node title.
    $node->title = chado_get_node_title($node);
    $nodes[$nid]->analysis = $analysis;
  }
}

/**
 * Implements hook_update().
 *
 * This function updates the chado_analysis_expression form.
 * An expression loader job may be submitted from an update.
 *
 * @ingroup tripal_analysis_expression
 */
function chado_analysis_expression_update($node) {

  // Call the analysis update to update the default analysis fields. 
  chado_analysis_update($node);

  // Remove white space around field values.
  $node->fileext = trim($node->fileext);
  $node->filepath = trim($node->filepath);
  $node->re_start = trim($node->re_start);
  $node->re_end = trim($node->re_end);

  $record = array('table' => 'analysis', 'id' => $node->analysis_id);
  $options = array('insert_if_missing' => TRUE);

  $assaydate = $node->assaydate['year'] . "-" . $node->assaydate['month'] . "-" . $node->assaydate['day'] . " 00:00:00";
  $acquisitiondate = $node->acquisitiondate['year'] . "-" . $node->acquisitiondate['month'] . "-" . $node->acquisitiondate['day'] . " 00:00:00";
  $quantificationdate = $node->quantificationdate['year'] . "-" . $node->quantificationdate['month'] . "-" . $node->quantificationdate['day'] . " 00:00:00";

  // Update the blast fields. 
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_organism_id',
    'cv_name' => 'tripal',
    'value' => trim($node->organism_id)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_associated_analysis_id',
    'cv_name' => 'tripal',
    'value' => trim($node->associated_analysis_id)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_biosourceprovider_id',
    'cv_name' => 'tripal',
    'value' => trim($node->biosourceprovider_id)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_arraydesign_id',
    'cv_name' => 'tripal',
    'value' => trim($node->arraydesign_id)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_assaydate',
    'cv_name' => 'tripal',
    'value' => $assaydate
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_assay_description',
    'cv_name' => 'tripal',
    'value' => trim($node->assay_description)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_acquisitiondate',
    'cv_name' => 'tripal',
    'value' => $acquisitiondate
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_acquisition_uri',
    'cv_name' => 'tripal',
    'value' => trim($node->acquisition_uri)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_quantificationdate',
    'cv_name' => 'tripal',
    'value' => $quantificationdate
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_quantification_uri',
    'cv_name' => 'tripal',
    'value' => trim($node->quantification_uri)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_assay_protocol_id',
    'cv_name' => 'tripal',
    'value' => trim($node->assay_protocol_id)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_acquisition_protocol_id',
    'cv_name' => 'tripal',
    'value' => trim($node->acquisition_protocol_id)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_quantification_protocol_id',
    'cv_name' => 'tripal',
    'value' => trim($node->quantification_protocol_id)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_assay_operator_id',
    'cv_name' => 'tripal',
    'value' => trim($node->assay_operator_id)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_quantification_operator_id',
    'cv_name' => 'tripal',
    'value' => trim($node->quantification_operator_id)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_filetype',
    'cv_name' => 'tripal',
    'value' => trim($node->filetype)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_submit_job',
    'cv_name' => 'tripal',
    'value' => trim($node->submit_job)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_fileext',
    'cv_name' => 'tripal',
    'value' => trim($node->fileext)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_filepath',
    'cv_name' => 'tripal',
    'value' => trim($node->filepath)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_re_start',
    'cv_name' => 'tripal',
    'value' => trim($node->re_start)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_re_end',
    'cv_name' => 'tripal',
    'value' => trim($node->re_end)
  ), $options);
  chado_update_property($record, array(
    'type_name' => 'analysis_expression_feature_uniquenames',
    'cv_name' => 'tripal',
    'value' => trim($node->feature_uniquenames)
  ), $options);

  if ($node->submit_job) {
    // Submit the expression loading job.
    chado_analysis_expression_submit_job($node);
  }
}

/**
 * Implements the tripal_add_job function for expression data.
 *
 * @ingroup tripal_analysis_expression
 */
function chado_analysis_expression_submit_job($node) {

  global $user;

  // Add the expression loader job to the job queue. 
  if ($node->submit_job) {

    $job_args = array(
      $node->organism_id,
      $node->analysis_id,
      $node->biosourceprovider_id,
      $node->arraydesign_id,
      $node->assaydate,
      $node->acquisitiondate,
      $node->quantificationdate,
      $node->filepath,
      $node->fileext,
      $node->filetype,
      $node->re_start,
      $node->re_end,
      $node->feature_uniquenames,
    );
    tripal_add_job("Loading expression data from: $node->filepath", 'tripal_analysis_expression',
      'tripal_expression_loader', $job_args, $user->uid);
  }
}

/**
 * Implements hook_node_view().
 *
 * @ingroup tripal_analysis_expression
 */
function tripal_analysis_expression_node_view($node, $view_mode, $langcode) {
  switch ($node->type) {
    case 'chado_analysis_expression':
      if ($view_mode == 'full') {
        $node->content['tripal_analysis_expression_base'] = array(
          '#markup' => theme('tripal_analysis_expression_base', array('node' => $node)),
          '#tripal_toc_id' => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );
        $node->content['tripal_analysis_expression_protocols'] = array(
          '#markup' => theme('tripal_analysis_expression_protocols', array('node' => $node)),
          '#tripal_toc_id' => 'protocols',
          '#tripal_toc_title' => 'Protocols',
          '#weight' => -80,
        );
        $node->content['tripal_analysis_biomaterial'] = array(
          '#markup' => theme('tripal_analysis_biomaterial', array('node' => $node)),
          '#tripal_toc_id' => 'biomaterial_browser', // Fix ****
          '#tripal_toc_title' => 'Biomaterial Browser',
          '#weight' => -9,
        );
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_analysis_expression_teaser'] = array(
          '#markup' => theme('tripal_analysis_expression_teaser', array('node' => $node)),
        );
      }
      break;

    case 'chado_arraydesign':
      if ($view_mode == 'full') {
        $node->content['tripal_arraydesign_base'] = array(
          '#markup' => theme('tripal_arraydesign_base', array('node' => $node)),
          '#tripal_toc_id' => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );
        $node->content['tripal_arraydesign_properties'] = array(
          '#markup' => theme('tripal_arraydesign_properties', array('node' => $node)),
          '#tripal_toc_id' => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_arraydesign_teaser'] = array(
          '#markup' => theme('tripal_arraydesign_teaser', array('node' => $node)),
        );
      }
      break;

    case 'chado_protocol':
      if ($view_mode == 'full') {
        $node->content['tripal_protocol_base'] = array(
          '#markup' => theme('tripal_protocol_base', array('node' => $node)),
          '#tripal_toc_id' => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );
        $node->content['tripal_protocol_properties'] = array(
          '#markup' => theme('tripal_protocol_properties', array('node' => $node)),
          '#tripal_toc_id' => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_protocol_teaser'] = array(
          '#markup' => theme('tripal_protocol_teaser', array('node' => $node)),
        );
      }
      break;

    case 'chado_feature':
      if ($view_mode == 'full') {
        $node->content['tripal_feature_expression'] = array(
          '#markup' => theme('tripal_feature_expression', array('node' => $node)),
          '#tripal_toc_id' => 'figure',
          '#tripal_toc_title' => 'Expression',
          '#weight' => -99,
        );
      }
      break;

    case 'chado_organism':
      if ($view_mode == 'full') {
        $node->content['tripal_organism_biomaterial'] = array(
          '#markup' => theme('tripal_organism_biomaterial', array('node' => $node)),
          '#tripal_toc_id' => 'biomaterial_browser', // Fix ****
          '#tripal_toc_title' => 'Biomaterial Browser',
          '#weight' => -9,
        );
      }
      break;

    case 'chado_biomaterial':
      if ($view_mode == 'full') {
        $node->content['tripal_biomaterial_properties'] = array(
          '#markup' => theme('tripal_biomaterial_properties', array('node' => $node)),
          '#tripal_toc_id' => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
        $node->content['tripal_biomaterial_base'] = array(
          '#markup' => theme('tripal_biomaterial_base', array('node' => $node)),
          '#tripal_toc_id' => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );
        $node->content['tripal_biomaterial_references'] = array(
          '#markup' => theme('tripal_biomaterial_references', array('node' => $node)),
          '#tripal_toc_id' => 'references',
          '#tripal_toc_title' => 'Cross References',
          '#weight' => -99,
        );
      }
      if ($view_mode == 'teaser') {
        $node->content['tripal_biomaterial_teaser'] = array(
          '#markup' => theme('tripal_biomaterial_teaser', array('node' => $node)),
        );
      }
      break;
  }

}

/**
 * Implements hook_node_insert().
 * Acts on all content types.
 *
 * @ingroup tripal_analysis_expression
 */
function tripal_analysis_expression_node_insert($node) {
  switch ($node->type) {
    case 'chado_analysis_expression':

      // Find the a analysis_expression and add details.     
      //$analysis_expression_id = chado_get_id_from_nid('analysis', $node->nid, 'chado.analysis');
      $analysis_expression_id = chado_get_id_from_nid('analysis', $node->nid);
      $values = array('analysis_id' => $analysis_expression_id);
      $analysis_expression = chado_generate_var('analysis', $values);
      $node->analysis_expression = $analysis_expression;

      // Set the node title.
      $node->title = chado_get_node_title($node);

      // Set the URL path.  
      chado_set_node_url($node);
      break;

    case 'chado_arraydesign':

      // Find the a arraydesign and add details.     
      $arraydesign_id = chado_get_id_from_nid('arraydesign', $node->nid);
      $values = array('arraydesign_id' => $arraydesign_id);
      $arraydesign = chado_generate_var('arraydesign', $values);
      $node->arraydesign = $arraydesign;

      // Set the node title.  
      $node->title = chado_get_node_title($node);

      // Set the URL path.  
      chado_set_node_url($node);

      break;

    case 'chado_protocol':

      // Find the a protocol and add details.     
      $protocol_id = chado_get_id_from_nid('protocol', $node->nid);
      $values = array('protocol_id' => $protocol_id);
      $protocol = chado_generate_var('protocol', $values);
      $node->protocol = $protocol;

      // Set the node title.  
      $node->title = chado_get_node_title($node);

      // Set the URL path.  
      chado_set_node_url($node);

      break;

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
 * @ingroup tripal_analysis_expression
 */
function tripal_analysis_expression_node_update($node) {

  switch ($node->type) {
    case 'chado_analysis_expression':

      // Set the node title.  
      $node->title = chado_get_node_title($node);

      // Set the URL path.  
      chado_set_node_url($node);

      break;

    case 'chado_arraydesign':

      // Set the node title.  
      $node->title = chado_get_node_title($node);

      // Set the URL path.  
      chado_set_node_url($node);

      break;

    case 'chado_biomaterial':

      // Set the node title.  
      $node->title = chado_get_node_title($node);

      // Set the URL path.  
      chado_set_node_url($node);

      break;

    case 'chado_protocol':

      // Set the node title.  
      $node->title = chado_get_node_title($node);

      // Set the URL path.  
      chado_set_node_url($node);

      break;
  }

}

function chado_analysis_expression_delete($node) {

  $analysis_id = chado_get_id_from_nid('analysis', $node->nid);

  /*  $sql = "SELECT E.element_id FROM chado.element E
             INNER JOIN chado.elementresult ER ON E.element_id = ER.element_id
             INNER JOIN chado.quantification Q ON ER.quantification_id = Q.quantification_id
           WHERE
             Q.analysis_id = :analysis_id";
    $element_ids = chado_query($sql, array(":analysis_id" => $analysis_id));

    foreach($element_ids as  $element_id) {
      chado_delete_record('element', array('element_id' => $element_id->element_id));
    } */

  $sql = "SELECT AY.assay_id FROM chado.assay AY
           INNER JOIN {acquisition} ACQ ON ACQ.assay_id = AY.assay_id
           INNER JOIN {quantification} Q ON ACQ.acquisition_id = Q.acquisition_id
         WHERE
           Q.analysis_id = :analysis_id";
  $assay_ids = chado_query($sql, array(":analysis_id" => $analysis_id));

  foreach ($assay_ids as $assay_id) {
    chado_delete_record('assay', array('assay_id' => $assay_id->assay_id));
  }

  $sql = "DELETE FROM {element} E
           WHERE NOT EXISTS 
         (SELECT * FROM {elementresult} ER 
           WHERE E.element_id = ER.element_id)";
  chado_query($sql);

  chado_analysis_delete($node);
}

function tripal_analysis_expression_node_presave($node) {
  switch ($node->type) {
    case 'chado_arraydesign':
      if (property_exists($node, 'arraydesign_name')) {
        $node->title = $node->arraydesign_name;
      }
      else {
        if (property_exists($node, 'arraydesign')) {
          $node->title = $node->arraydesign->name;
        }
      }
      break;

    case 'chado_protocol':
      if (property_exists($node, 'protocol_name')) {
        $node->title = $node->protocol_name;
      }
      else {
        if (property_exists($node, 'protocol')) {
          $node->title = $node->protocol->name;
        }
      }
      break;

    case 'chado_biomaterial':
      if (property_exists($node, 'biomaterial_name')) {
        $node->title = $node->biomaterial_name;
      }
      else {
        if (property_exists($node, 'biomaterial')) {
          $node->title = $node->biomaterial->name;
        }
      }
      break;
  }
}



















