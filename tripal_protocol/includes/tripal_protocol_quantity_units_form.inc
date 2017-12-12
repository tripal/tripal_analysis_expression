<?php

/**
 * Units form.
 *
 * @implements HOOK_form()
 *
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */

function tripal_protocol_quantity_units_form($form, &$form_state) {
  $form['units_description'] = [
    '#type' => 'markup',
    '#markup' => '<h3>Quantification Units</h3>
<p>The <b>Quantification</b> step of an experiment is where raw data is transformed and analyzed.  This form allows you to manage the units associated with your quantifications.  These units can, for example, be displayed alongside microarray or RNAseq data in the tripal_analysis_expression module.  For a conceptual map of biomaterial/biosample, protocol, assay, acquisition, quantification, and elements, see the  <a href="http://gmod.org/wiki/Chado_Mage_Module#Table:_quantification">Chado mage module documentation.</a></p>
',
  ];

  $form['new_label'] = [
    '#type' => 'textfield',
    '#title' => t('Assign units'),
    '#description' => t('Assign units to ALL quantifications without units.  If you add new quantifications without units, you must rerun this command.  Quantifications that have units assigned will not be affected.  Note, this cannot be undone.'),
    '#default_value' => t("FPKM"),
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Submit',
  ];

  //Get list of all quantifications.

  $type_id = tripal_get_cvterm([
    'name' => 'unit_of_measure',
    'cv_id' => ['name' => 'sep'],
  ])->cvterm_id;

  // fetch all quantifications.  Also get the quantification props matching unit_of_measure, but include those without quantificationprops.

  $sql = "SELECT Q.quantification_id, Q.name, QP.value FROM {quantification} Q 
   FULL JOIN {quantificationprop} QP ON Q.quantification_id = QP.quantification_id 
   WHERE QP.type_id = :type_id OR QP.type_id IS null";
  $results = chado_query($sql, [':type_id' => $type_id])->fetchAll();

  $header = ['Quantification ID', 'Quantification Name', 'Unit label', 'Edit'];
  $rows = [];

  foreach ($results as $result) {
    $id = $result->quantification_id;
    $quantification_name = $result->name;
    $unit_label = $result->value;
    $edit_link = '<a href = "quantification_units/edit/' . $id . '"> edit </a>';
    $rows[] = [
      'Quantification ID' => $id,
      'Quantification Name' => $quantification_name,
      'Unit label' => $unit_label,
      'Edit' => $edit_link,
    ];
  }

  $table = [
    'header' => $header,
    'rows' => $rows,
    'attributes' => [],
    'sticky' => FALSE,
    'caption' => 'Individual quantifications and their units can be managed here.',
    'colgroups' => [],
    'empty' => '',
  ];
  $output = theme_table($table);

  //pagination settings
  $per_page = 20;
  // Initialize the pager
  $current_page = pager_default_initialize(count($rows), $per_page);
  // Split your list into page sized chunks
  $chunks = array_chunk($rows, $per_page, TRUE);
  // Show the appropriate items from the list
  $output = theme('table', [
    'header' => $header,
    'rows' => $chunks[$current_page],
  ]);
  // Show the pager
  $output .= theme('pager', ['quantity', count($rows)]);

  $form['Table'] = [
    '#type' => 'markup',
    '#markup' => $output,
  ];

  return $form;
}

function tripal_protocol_quantity_units_form_submit($form, &$form_state) {

  global $user;

  $new_label = $form_state['values']['new_label'];
  $args["new_label"] = $new_label;

  tripal_add_job("Set units for unset quantifications", 'tripal_protocol', 'tripal_protocol_quantity_units_batch_alter', $args, $user->uid, 10);

  drupal_goto('admin/tripal/extension/tripal_protocol/quantification_units');
}

/**
 * Allows user to edit the units associated with a quantification.
 *
 * @param $form
 * @param $form_state
 * @param $quantification_id
 *
 * @return mixed
 */
function tripal_protocol_quantity_units_edit_form(
  $form,
  &$form_state,
  $quantification_id
) {


  $type_id = tripal_get_cvterm([
    'name' => 'unit_of_measure',
    'cv_id' => ['name' => 'sep'],
  ])->cvterm_id;

  $sql = "SELECT Q.quantification_id, Q.name, QP.value FROM {quantification} Q 
   INNER JOIN {quantificationprop} QP ON Q.quantification_id = QP.quantification_id 
   WHERE Q.quantification_id = :quantification_id AND QP.type_id = :type_id";

  $result = chado_query($sql, [
    ':type_id' => $type_id,
    ':quantification_id' => $quantification_id,
  ])->fetchObject();

  $unit_label = $result->value;

  $form['quantification_id'] = [
    '#type' => 'value',
    '#value' => $quantification_id,
  ];

  $form['unit_label'] = [
    '#type' => 'textfield',
    '#title' => t('Assign quantification units'),
    '#description' => t('Enter a new unit label to associate with this quantification.  Labels should be plural (ie RPKM, counts).'),
    '#default_value' => $unit_label,
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Submit Edit',
  ];
  return $form;
}

/**Submit edit for a single quantification.
 *
 * @param $form
 * @param $form_state
 */
function tripal_protocol_quantity_units_edit_form_submit($form, &$form_state) {
  $quantification_id = $form_state['values']['quantification_id'];
  $unit_label = $form_state['values']['unit_label'];
  $result = chado_update_property([
    "table" => "quantification",
    "id" => $quantification_id,
  ], [
    'type_name' => 'unit_of_measure',
    'cv_name' => 'sep',
    'value' => $unit_label,
  ], ['insert_if_missing' => TRUE]);

  if ($result) {
    drupal_set_message("Success: updated units for quantification " . $quantification_id);
  }
  else {
    drupal_set_message("Error: Could not set units for quantification " . $quantification_id, "error");
  }
  drupal_goto('admin/tripal/extension/tripal_protocol/quantification_units');
}

/**Submits job to batch update units for all unset quantifications
 *
 * @param $args
 */
function tripal_protocol_quantity_units_batch_alter($args) {

  $unit_label = $args;
  //Get all quantifications without unit properties set (Left outer with is null)
  $sql = "SELECT Q.quantification_id FROM {quantification} Q 
    LEFT OUTER JOIN {quantificationprop} QP ON Q.quantification_id = QP.quantification_id
    WHERE QP.quantification_id IS null";

  $results = chado_query($sql)->fetchAll();
  $count = count($results);

  drush_print("Assigning the units " . $unit_label . " to " . $count . " quantifications.");

  foreach ($results as $quantification) {
    $quantification_id = $quantification->quantification_id;
    $result = chado_update_property([
      "table" => "quantification",
      "id" => $quantification_id,
    ], [
      'type_name' => 'unit_of_measure',
      'cv_name' => 'sep',
      'value' => $unit_label,
    ], ['insert_if_missing' => TRUE]);
    if (!$result) {
      drush_print("Warning: could not update quantification " . $quantification_id);
    }
  }
}