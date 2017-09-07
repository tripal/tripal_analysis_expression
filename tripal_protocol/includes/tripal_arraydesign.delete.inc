<?php
/**
 * @file
 * Administration interface for deleting multiple arraydesigns.
 */

/**
 * A form for indicating the arraydesigns to delete.
 *
 * @ingroup tripal_arraydesign
 */
function tripal_arraydesign_delete_form() {
  // Get the list of arraydesigns.
  $sql = "SELECT * FROM {arraydesign} ORDER BY name";
  $arraydesign_rset = chado_query($sql);
  $arraydesigns = [];
  while ($arraydesign = $arraydesign_rset->fetchObject()) {
    $arraydesigns[$arraydesign->arraydesign_id] = "$arraydesign->name";
  }

  // Create the delete form.
  $form['desc'] = [
    '#markup' => t("Use the following form to delete array designs which may or may
        not be synced."),
  ];

  $form['arraydesigns'] = [
    '#title' => t('Array Design'),
    '#type' => 'checkboxes',
    '#options' => $arraydesigns,
    '#prefix' => '<div style="height: 400px; overflow: scroll">',
    '#suffix' => '</div><br>',
  ];

  $form['button'] = [
    '#type' => 'submit',
    '#value' => t('Delete Array Designs'),
  ];
  return $form;
}

/**
 * Submit the delete arraydesigns form.
 *
 * @ingroup tripal_arraydesign
 */
function tripal_arraydesign_delete_form_submit($form, &$form_state) {
  global $user;

  // Convert the arraydesigns input into an array of just ids.
  $arraydesigns = $form_state['values']['arraydesigns'];
  $arraydesign_ids = [];
  foreach ($arraydesigns as $id => $val) {
    if ($val != 0) {
      $arraydesign_ids[] = $id;
    }
  }

  if (count($arraydesign_ids) > 0) {
    $args = [$arraydesign_ids];

    tripal_add_job("Delete Array Design", 'tripal_analysis_expression', 'tripal_arraydesign_delete_arraydesigns', $args, $user->uid);
    drupal_set_message(t("Array Designs will disappear from the list below once the job completes."));
  }
  else {
    drupal_set_message(t("Please select at least one array design to delete."), "error");
  }
}

/**
 * Function that is called by tripal jobs that actually delete the
 *  indicated arraydesigns.
 *
 * @param $arraydesign_id
 *   The list or arraydesign_ids of the arraydesigns to delete.
 * @param $job
 *   The tripal_job id
 *
 * @ingroup tripal_arraydesign
 */
function tripal_arraydesign_delete_arraydesigns($arraydesigns, $job = NULL) {

  global $user;

  // Begin the database transaction.
  $transaction = db_transaction();
  print "\nNOTE: Deleting array designs is performed using a database transaction. \n" . "If the delete fails or is terminated prematurely then the entire set of \n" . "deletions is rolled back the array designs will continue to be found in the database\n\n";

  try {
    $values = [
      'arraydesign_id' => $arraydesigns,
    ];
    $num_deletes = chado_select_record('arraydesign', ['count(*) as cnt'], $values);
    print "Deleting " . $num_deletes[0]->cnt . " arraydesigns\n";
    chado_delete_record('arraydesign', $values);

    print "Removing orphaned arraydesign pages\n";
    chado_cleanup_orphaned_nodes('arraydesign', $num_deletes[0]->cnt, 'chado_arraydesign', 'chado_arraydesign');
  } catch (Exception $e) {
    print "\n";
    $transaction->rollback();
    print "FAILED: Rolling back database changes...\n";
    watchdog_exception('tripal_arraydesign', $e); //*** does this need to be tripal_analsis_exp..
    return 0;
  }
  print "\nDONE\n";
}

