<?php
/**
 * @file
 * Administration interface for deleting multiple protocols.
 */

/**
 * A form for indicating the protocols to delete.
 *
 * @ingroup tripal_protocol
 */
function tripal_protocol_delete_form() {
  // Get the list of protocols.
  $sql = "SELECT * FROM {protocol} ORDER BY name";
  $protocol_rset = chado_query($sql);
  $protocols = [];
  while ($protocol = $protocol_rset->fetchObject()) {
    $protocols[$protocol->protocol_id] = "$protocol->name";
  }

  // Create the delete form.
  $form['desc'] = [
    '#markup' => t("Use the following form to delete protocols which may or may
        not be synced."),
  ];

  $form['protocols'] = [
    '#title' => t('Protocol'),
    '#type' => 'checkboxes',
    '#options' => $protocols,
    '#prefix' => '<div style="height: 400px; overflow: scroll">',
    '#suffix' => '</div><br>',
  ];

  $form['button'] = [
    '#type' => 'submit',
    '#value' => t('Delete Protocols'),
  ];
  return $form;
}

/**
 * Submit the delete protocols form.
 *
 * @ingroup tripal_protocol
 */
function tripal_protocol_delete_form_submit($form, &$form_state) {
  global $user;

  // Convert the protocols input into an array of just ids.
  $protocols = $form_state['values']['protocols'];
  $protocol_ids = [];
  foreach ($protocols as $id => $val) {
    if ($val != 0) {
      $protocol_ids[] = $id;
    }
  }

  if (count($protocol_ids) > 0) {
    $args = [$protocol_ids];

    tripal_add_job("Delete Protocol", 'tripal_analysis_expression', 'tripal_protocol_delete_protocols', $args, $user->uid);
    drupal_set_message(t("Protocols will disappear from the list below once the job completes."));
  }
  else {
    drupal_set_message(t("Please select at least one protocol to delete."), "error");
  }
}

/**
 * Function that is called by tripal jobs that actually delete the
 * indicated protocols.
 *
 * @param $protocol_id
 *   The list or protocol_ids of the protocols to delete.
 * @param $job
 *   The tripal_job id
 *
 * @ingroup tripal_protocol
 */
function tripal_protocol_delete_protocols($protocols, $job = NULL) {

  global $user;

  // Begin the database transaction.
  $transaction = db_transaction();
  print "\nNOTE: Deleting protocols is performed using a database transaction. \n" . "If the delete fails or is terminated prematurely then the entire set of \n" . "deletions is rolled back the protocols will continue to be found in the database\n\n";

  try {
    $values = [
      'protocol_id' => $protocols,
    ];
    $num_deletes = chado_select_record('protocol', ['count(*) as cnt'], $values);
    print "Deleting " . $num_deletes[0]->cnt . " protocols\n";
    chado_delete_record('protocol', $values);

    print "Removing orphaned protocol pages\n";
    chado_cleanup_orphaned_nodes('protocol', $num_deletes[0]->cnt, 'chado_protocol', 'chado_protocol');
  } catch (Exception $e) {
    print "\n";
    $transaction->rollback();
    print "FAILED: Rolling back database changes...\n";
    watchdog_exception('tripal_protocol', $e); //*** does this need to be tripal_analsis_exp..
    return 0;
  }
  print "\nDONE\n";
}

