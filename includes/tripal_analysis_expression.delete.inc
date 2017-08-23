<?php
/**
 * @file
 * Administration interface for deleting multiple analysis expressions.
 */

/**
 * A form for indicating the analysis expressions to delete.
 *
 * @ingroup tripal_analysis_expression
 */
function tripal_analysis_expression_delete_form() {
  // Get the list of analysis expressions.
  $sql = "SELECT A.analysis_id, A.name FROM {analysis} A 
           INNER JOIN {analysisprop} AP ON A.analysis_id = AP.analysis_id
           INNER JOIN {cvterm} CVT ON AP.type_id = CVT.cvterm_id
         WHERE
           CVT.name = 'analysis_expression_organism_id' 
         ORDER BY A.name";

  $analysis_expression_rset = chado_query($sql);
  $analysis_expressions = [];
  while ($analysis_expression = $analysis_expression_rset->fetchObject()) {
    $analysis_expressions[$analysis_expression->analysis_id] = "$analysis_expression->name";
  }

  // Create the delete form.
  $form['desc'] = [
    '#markup' => t("Use the following form to delete Analysis: Expressions which may or may
        not be synced."),
  ];

  $form['analysis_expressions'] = [
    '#title' => t('Analysis: Expression'),
    '#type' => 'checkboxes',
    '#options' => $analysis_expressions,
    '#prefix' => '<div style="height: 400px; overflow: scroll">',
    '#suffix' => '</div><br>',
  ];

  $form['button'] = [
    '#type' => 'submit',
    '#value' => t('Delete Analysis: Expressions'),
  ];
  return $form;
}

/**
 * Submit the delete analysis_expressions form.
 *
 * @ingroup tripal_analysis_expression
 */
function tripal_analysis_expression_delete_form_submit($form, &$form_state) {
  global $user;

  // Convert the analysis_expressions input into an array of just ids.
  $analysis_expressions = $form_state['values']['analysis_expressions'];
  $analysis_ids = [];
  foreach ($analysis_expressions as $id => $val) {
    if ($val != 0) {
      $analysis_ids[] = $id;
    }
  }

  if (count($analysis_ids) > 0) {
    $args = [$analysis_ids];

    tripal_add_job("Delete Analysis: Expression", 'tripal_analysis_expression', 'tripal_analysis_expression_delete_analysis_expressions', $args, $user->uid);
    drupal_set_message(t("Analysis: Expressions will disappear from the list below once the job completes."));
  }
  else {
    drupal_set_message(t("Please select at least one Analysis: Expression to delete."), "error");
  }
}

/**
 * Function that is called by tripal jobs that actually deletes the
 * indicated analysis_expressions.
 *
 * @param $analysis_id
 *   The list or analysis_ids of the analysis_expressions to delete.
 * @param $job
 *   The tripal_job id
 *
 * @ingroup tripal_analysis_expression
 */   ///***consistant in name of delete function parameters
function tripal_analysis_expression_delete_analysis_expressions($analysis, $job = NULL) {

  global $user;

  // Begin the database transaction.
  $transaction = db_transaction();
  print "\nNOTE: Deleting Analysis: Expressions is performed using a database transaction. \n" . "If the delete fails or is terminated prematurely then the entire set of \n" . "deletions is rolled back the Analysis: Expressions will continue to be found in the database\n\n";

  try {
    $values = [
      'analysis_id' => $analysis,
    ];
    $num_deletes = chado_select_record('analysis', ['count(*) as cnt'], $values);
    print "Deleting " . $num_deletes[0]->cnt . " analysis_expressions\n";
    chado_delete_record('analysis', $values);

    print "Removing orphaned analysis_expression pages\n";
    chado_cleanup_orphaned_nodes('analysis', $num_deletes[0]->cnt, 'chado_analysis', 'chado_analysis');
  } catch (Exception $e) {
    print "\n";
    $transaction->rollback();
    print "FAILED: Rolling back database changes...\n";
    watchdog_exception('tripal_analysis', $e);
    return 0;
  }
  print "\nDONE\n";
}
