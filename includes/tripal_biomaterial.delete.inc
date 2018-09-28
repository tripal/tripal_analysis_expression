<?php
/**
 * @file 
 * Administration interface for deleting multiple biomaterials.
 */ 

/**
 * A form for indicating the biomaterials to delete.
 * 
 * @ingroup tripal_biomaterial
 */
function tripal_biomaterial_delete_form() {
  // Get the list of biomaterials.
  $sql = "SELECT * FROM {biomaterial} ORDER BY name";
  $biomaterial_rset = chado_query($sql);
  $biomaterials = array();
  while ($biomaterial = $biomaterial_rset->fetchObject()) {
    $biomaterials[$biomaterial->biomaterial_id] = "$biomaterial->name";
  }


  // Create the delete form.
  $form['desc'] = array(
    '#markup' => t("Use the following form to delete biomaterials which may or may
        not be synced."),
  );

  $form['biomaterial_names'] = array(
    '#type' => 'textarea', 
    '#title' => t('Biomaterial Names'),
    '#description' => t('Please provide a list of biomaterial names
       (separated by spaces or new lines) to be deleted. If you specify biomaterial
       names then all other options below will be ignored.'),
   );

  $organisms = tripal_get_organism_select_options(FALSE); 
  $form['organism_id'] = array(
    '#title' => t('Organism'),
    '#type' => 'select',
    '#description' => t("Choose the organism for which biomaterials will be deleted."),
    '#options' => $organisms,
  );

  $analyses = tripal_get_analysis_select_options(FALSE);
  $form['analysis_id'] = array(
    '#title' => t('Analysis'),
    '#type' => 'select',
    '#description' => t("Choose the organism for which biomaterials will be deleted"),
    '#options' => $analyses,
  );
  
/*
  $form['biomaterials'] = array(
    '#title' => t('Biomaterial'),
    '#type' => 'checkboxes',
    '#options' => $biomaterials,
    '#prefix' => '<div style="height: 400px; overflow: scroll">',
    '#suffix' => '</div><br>',
  );
*/
  $form['button'] = array(
    '#type' => 'submit',
    '#value' => t('Delete Biomaterials'),
  );
  return $form;
}

/**
 * Validation for the delete biomaterials form.
 * 
 * @ingroup tripal_biomaterial
 */
function tripal_biomaterial_delete_from_validate($form, &$form_state) { 
  $biomaterial_names = $form_state['values']['biomaterial_names'];
  $organism_id = $form_state['values']['organism_id'];
  $analysis_id = $form_state['values']['analysis_id'];

  if (!$biomaterial_names and !$organism_id and !$analysis_id) {
    form_set_error('biomaterial_names', t("Please select at least one option"));
  }
} 

/**
 * Submit the delete biomaterials form.
 * 
 * @ingroup tripal_biomaterial 
 */
function tripal_biomaterial_delete_form_submit($form, &$form_state) {
  global $user;

  $biomaterial_names = $form_state['values']['biomaterial_names'];
  $organism_id = $form_state['values']['organism_id'];
  $analysis_id = $form_state['values']['analysis_id'];

  $args = array($biomaterial_names, $organism_id, $analysis_id);
  tripal_add_job("Delete biomaterials", 'tripal_biomaterial',
    'tripal_biomaterial_delete_biomaterials', $args, $user->uid);
}

/**
 * Function that is called by tripal jobs that actually delete the 
 *  indicated biomaterials.
 * 
 * @param $biomaterial_names
 *   (Optional) The list of biomaterial names to delete.
 * @param $organism_id
 *   (Optional) The organism_id of the biomaterials to delete. 
 * @param $analysis_id
 *   (Optional) The analysis_id of the biomaterials to delete. 
 * @param $job
 *   The tripal_job id
 * 
 * @ingroup tripal_biomaterial
 */
function tripal_biomaterial_delete_biomaterials($biomaterial_names, $organism_id, $analysis_id, $job = NULL) {

  global $user;

  // Begin the database transaction.
  $transaction = db_transaction();
  print "\nNOTE: Deleting biomaterials is performed using a database transaction. \n" . 
      "If the delete fails or is terminated prematurely then the entire set of \n" . 
      "deletions is rolled back the biomaterials will continue to be found in the database\n\n";

  try {
    // If biomaterial names have been provided then handle those.
    if ($biomaterial_names) {
      $names = preg_split('/\s+/', $biomaterial_names);
      if (sizeof($names) == 1) { 
        $names = $names[0];
      }
      $match['name'] = $names;
      $num_deletes = chado_select_record('biomaterial', array('count(*) as cnt'), $match);
      print "Deleting " . $num_deletes[0]->cnt . " features\n";

      chado_delete_record('biomaterial', $match);
      return;
    }

    // Now handle the combination of all other inputs. 
    $args = array();
    $sql = "";
    $join = "";
    $where = "";
    if ($organism_id) { 
      $where .= 'AND B.taxon_id = :organism_id ';
      $args[':organism_id'] = $organism_id;
    }
    if ($analysis_id) {
      $join .= 'INNER JOIN {assay_biomaterial} AB ON B.biomaterial_id = AB.biomaterial_id ';
      $join .= 'INNER JOIN {assay} AY ON AB.assay_id = AY.assay_id ';
      $join .= 'INNER JOIN {acquisition} AC ON AY.assay_id = AC.assay_id ';
      $join .= 'INNER JOIN {quantification} Q ON AC.acquisition_id = Q.acquisition_id ';
      $where .= 'AND AC.analysis_id = :analysis_id';
      $args[':analysis_id'] = $analysis_id;
    }

    // Only perform a delete if the where clause is not empty. Other wise "1=1"
    // in the sql statement below will cause all biomaterials to be deleted.
    if (!$where) { 
      throw new Exception('Cannot delete biomaterials as no filters are available');
    }
    // First, count the number of records to be deleted. 
    $sql = "
      SELECT count(B.biomaterial_id)
      FROM {biomaterial} B
        $join
      WHERE 1=1 $where";
    $num_deleted = chado_query($sql, $args)->fetchField();
    // Second, delete the records.
    $sql = "
      DELETE FROM {biomaterial} WHERE biomaterial_id IN ( 
        SELECT B.biomaterial_id
        FROM {biomaterial} B
          $join
        WHERE 1=1 $where
      )";
    chado_query($sql,$args);
    print "Deletion completed successfully. Deleted $num_deleted biomaterial(s).\n";

    print "Now removing orphaned biomaterial pages\n";
    chado_cleanup_orphaned_nodes('biomaterial', $num_deleted, 'chado_biomaterial', 'chado_biomaterial');
  }
  catch (Exception $e) {
    print "\n"; 
    $transaction->rollback();
    print "FAILED: Rolling back database changes...\n";
    watchdog_exception('tripal_biomaterial', $e); //*** does this need to be tripal_analsis_exp..
    return 0;
  }
  print "\nDONE\n";
}

