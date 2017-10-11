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

  $form['assign_unset'] = [
    '#type' => 'textfield',
    '#title' => t('Assign default units'),
    '#description' => t('Assign units to all quantifications without units.  If you add new quantifications without units, you must rerun this command.  Quantifications that have units assigned will not be affected.'),
    '#default_value' => t("FPKM"),
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Save',
  ];

  //Get list of all quantifications.

  $type_id = tripal_get_cvterm([
    'name' => 'unit_of_measure',
    'cv_id' => ['name' => 'sep'],
  ])->cvterm_id;

  $sql = "SELECT Q.quantification_id, Q.name, QP.value FROM {quantification} Q 
   INNER JOIN {quantificationprop} QP ON Q.quantification_id = QP.quantification_id 
   WHERE QP.type_id = :type_id";

  $results = chado_query($sql, [':type_id' => $type_id])->fetchAll();

  $headers = ['Quantification ID', 'Quantification Name', 'Unit label', 'Edit'];
  $rows = [];

  foreach($results as $result){
    $id = $result->quantification_id;
    $quantification_name = $result->name;
    $unit_label = $result->value;
    $edit_link = 'edit';
    $rows[] = ['Quantification ID' => $id, 'Quantification Name' => $quantification_name, 'Unit label' => $unit_label, 'Edit' => $edit_link];
  }

  $table = [
    'header' => $headers,
    'rows' => $rows,
    'attributes' => [],
    'sticky' => FALSE,
    'caption' => 'Individual quantifications and their units can be managed here.',
    'colgroups' => [],
    'empty' => '',
  ];
  $output = theme_table($table);
  
  $form['Table'] = [
    '#type' => 'markup',
    '#markup' => $output,
  ];

  return $form;
}
