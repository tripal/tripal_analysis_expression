<?php

/**
 * Implements the function that will called as a tripal job
 *  to load expression data.
 *
 * @param $organism_id
 *  The organism_id of the organism from which the biomaterial was gathered.
 *
 * @param $analysis_id
 *  The analysis_id of the analysis: expression from which the expression
 *   loader is being called.
 *
 * @param $contact_id
 *  The contact_id of the contact that provided the biomaterial. This paramter
 *   is optional.
 *
 * @param $arraydesign_id
 *  The arraydesign_id of the arraydesign associated with the experiment. This
 *   parameter is optional.
 *
 * @param $assay_id
 *  The assay_id of the assay associated with the experiment. This parameter is
 *   optional.
 *
 * @param $acquisition_id
 *  The acquisition_id of the acquisition associated with the experiment. This
 *   parameter is optional.
 *
 * @param $quantificatin_id
 *  The quantification_id of the quantification associated with the experiment.
 *   This parameter is optional.
 *
 * @param $filepath
 *  The file path to the expression file/s to be loaded.
 *
 * @param $fileext
 *  The file extension of the expression files to be loaded.
 *
 * @param $filetype
 *  The file type can either by mat for matrix or col for column.
 *
 * @ingroup tripal_analysis_expression
 */

function tripal_expression_loader($organism_id, $analysis_id, $contact_id, $arraydesign_id, $assaydate, $acquisitiondate, $quantificationdate, $filepath, $fileext, $filetype, $re_start, $re_stop, $feature_uniquenames) {

  $features = [];

  $uniq_name = get_unique_name($organism_id, $analysis_id);

  $transaction = db_transaction();
  print "\nNOTE: Loading of this expression file is performed using a database transaction. \n" . "If the load fails or is terminated prematurely then the entire set of \n" . "insertions/updates is rolled back and will not be found in the database\n\n";
  try {
    //if(!$contact_id) {
    //  $contact_id = create_generic_contact(); //***
    // }
    // Create the array design. 
    $arraydesign_id = $arraydesign_id ? $arraydesign_id : create_generic_arraydesign(create_generic_contact());
    $re_ext = '/^.+\.(' . $fileext . ')/';

    // A matrix file.
    if ($filetype == 'mat' and is_file($filepath) and preg_match($re_ext, $filepath)) {
      tripal_expression_load_mat_file($filepath, $fileext, $arraydesign_id, $organism_id, $analysis_id, $contact_id, $assay_id, $acquisition_id, $quantification_id, $re_start, $re_stop, $uniq_name, $feature_uniquenames);
    }

    // The column format.
    elseif ($filetype == 'col' and is_file($filepath) and preg_match($re_ext, $filepath)) {
      $features = tripal_expression_load_col_file($filepath, $fileext, $arraydesign_id, $organism_id, $analysis_id, $contact_id, $assay_id, $acquisition_id, $quantification_id, $re_start, $re_stop, $uniq_name, $feature_uniquenames);
    }
    else { // Multiple files in the column format.
      $dir_handle = @opendir($filepath) or die("Unable to open $filepath");
      $total_files = count(glob($filepath . '/*.' . $fileext));
      $num_file = 0;

      // Load all files with of type $fileext in the directory.
      while ($file = readdir($dir_handle)) {
        if (preg_match('/(.+)\.' . $fileext . '$/', $file)) {
          print "File " . ($num_file + 1) . " of $total_files: $file                                       \n";
          $new_features = tripal_expression_load_col_file($filepath . '/' . $file, $fileext, $arraydesign_id, $organism_id, $analysis_id, $contact_id, $assay_id, $acquisition_id, $quantification_id, $re_start, $re_stop, $uniq_name, $feature_uniquenames);
          $num_file++;
          $old_features = $features;
          $features = (array_unique(array_merge($old_features, $new_features)));
        }
      }
    }

    // Associate features with an analysis.
    foreach ($features as $feature_id) {
      $analysisfeature_id = chado_query('SELECT analysisfeature_id FROM {analysisfeature} WHERE analysis_id = :analysis_id AND feature_id = :feature_id', [
        ':analysis_id' => $analysis_id,
        ':feature_id' => $feature_id,
      ])->fetchField();
      if (!$analysisfeature_id) {
        $analysisfeature = chado_insert_record('analysisfeature', [
          'analysis_id' => $analysis_id,
          'feature_id' => $feature_id,
        ]);
        if ($analysisfeature == FALSE) {
          print "ERROR: Unable to write to analysisfeature table\n";
          exit;
        }
      }
    }
  } catch (Exception $e) {
    print "\n";
    watchdog_exception('T_expression_load', $e);
    $transaction->rollback();
    print "FAILED: Rolling back database changes...\n";
  }
  print "\nDone.\n";

  return;
}

function get_unique_name($organism_id, $analysis_id) {

  $uniq_string = " from ";
  $sql = "SELECT common_name FROM {organism} WHERE organism_id = :organism_id";
  $uniq_string .= chado_query($sql, [':organism_id' => $organism_id])->fetchField();
  $uniq_string .= " for ";
  $sql = "SELECT name FROM {analysis} WHERE analysis_id = :analysis_id";
  $uniq_string .= chado_query($sql, [':analysis_id' => $analysis_id])->fetchField();
  print $uniq_string;
  return $uniq_string;
}

/**
 *
 * Implements the function that will parse an expression file with a matrix
 * file.
 *
 * @param $organism_id
 *  The organism_id of the organism from which the biomaterial was gathered.
 *
 * @param $analysis_id
 *  The analysis_id of the analysis: expression from which the expression
 *   loader is being called.
 *
 * @param $contact_id
 *  The contact_id of the contact that provided the biomaterial. This paramter
 *   is optional.
 *
 * @param $arraydesign_id
 *  The arraydesign_id of the arraydesign associated with the experiment. This
 *   parameter is optional.
 *
 * @param $assay_id
 *  The assay_id of the assay associated with the experiment. This parameter is
 *   optional.
 *
 * @param $acquisition_id
 *  The acquisition_id of the acquisition associated with the experiment. This
 *   parameter is optional.
 *
 * @param $quantificatin_id
 *  The quantification_id of the quantification associated with the experiment.
 *   This parameter is optional.
 *
 * @param $filepath
 *  The file path to the expression file/s to be loaded.
 *
 * @param $fileext
 *  The file extension of the expression files to be loaded.
 *
 * @ingroup tripal_analysis_expression
 */

function tripal_expression_load_mat_file($filepath, $fileext, $arraydesign_id, $organism_id, $analysis_id, $contact_id, $assay_id, $acquisition_id, $quantification_id, $re_start, $re_stop, $uniq_name, $feature_uniquenames) {

  $data_ln = 0;
  $mat_fp = fopen($filepath, 'r');
  $num_col = 0;
  $mat_head = 0;
  $num_features = 0;
  $num_lines = 0;
  $cur_feature = 1;
  $num_biomaterials = 0;

  $quant_array = [];
  $feature_array = [];

  // Count the number of features in the file.
  // Also make sure there are no duplicate features.

  // Ignore the feature footer.
  while ($line = fgets($mat_fp)) {
    $num_lines++;
    if (preg_match('/' . $re_stop . '/', $line) and $re_stop) {
      $data_ln = 0;
    }
    // Grab data lines. Ingore the header.
    if ($data_ln == 1 or !$re_start) {
      if ($mat_head == 0) {
        $num_features++;
        $mat_head = 1;
        continue;
      }
      elseif ($mat_head == 1) {
        $num_features++;
        preg_match("/^(.+?)\t/", $line, $match);
        $feature_array[] = trim($match[1]);
      }
    }
    // Only capture lines after the header. 
    if (preg_match('/' . $re_start . '/', $line) and $re_start) {
      $data_ln = 1;
    }
  }

  // Make sure there not duplicate feature names.
  if (count($feature_repeats = array_diff_assoc($feature_array, array_unique($feature_array))) > 0) {
    //if (count($linepart) != count(array_unique($linepart))) {
    print "ERROR: There are multiple instance of the following " . "feature(s) in the expression file:\n";
    foreach ($feature_repeats as $repeat) {
      print "  " . $repeat . "\n";
    }
    print "Please make sure there is only one instance of each " . "feature in the expression file\n";
    exit;
  }

  // Get rid of the feature_array. 
  unset($feature_array);

  //fclose($mat_fp);
  // Rewind the file.
  rewind($mat_fp);
  $mat_head = 0;

  $mat_fp = fopen($filepath, 'r');

  $set_fp = '';

  $load_limit = 500;

  // Calculate interval length.
  $int_length = .01 * $num_features;

  // The minimum interval length is 1.
  if ($int_length < 1) {
    $int_length = 1;
  }

  // Loop through every line in a file.
  //while($line = fgets($mat_fp) and (intval(ftell($mat_fp)) < $num_lines) {

  print "Loading complete: " . 0 . "%. Memory: " . number_format(memory_get_usage()) . " bytes. \r";

  for ($j = 0; $j < $num_lines and $line = fgets($mat_fp); $j++) {

    if ($j % $load_limit == 0) {
      $set_fp = ftell($mat_fp);
      fclose($mat_fp);
      $time_end = microtime(TRUE);
      if ($j != 0) {
        print $load_limit / ($time_end - $time_start) . " lines per second\n";
      }
      $time_start = microtime(TRUE);
      $mat_fp = fopen($filepath, 'r');
      fseek($mat_fp, $set_fp);
    }

    if (preg_match('/' . $re_stop . '/', $line) and $re_stop) {
      $data_ln = 0;
    }
    if ($data_ln == 1 or !$re_start) {
      $linepart = preg_split('/\t/', $line, -1, PREG_SPLIT_NO_EMPTY);
      // Trim whitespace from columns.
      $new_linepart = [];
      foreach ($linepart as $col) {
        $new_linepart[] = trim($col);
      }
      // keep zero values, but get rid of possible null, false, and empty strings
      $linepart = array_filter($new_linepart, 'strlen');
      $num_col = count($linepart);
      $linepart = array_values($linepart);

      if ($mat_head == 0) {
        foreach ($linepart as $sample) {
          $quant_array[] = expression_create_biomaterial_structure($sample, $organism_id, $analysis_id, $contact_id, $arraydesign_id, $assay_id, $acquisition_id, $quantification_id, $uniq_name);
        }

        // Make sure there not duplicate biomaterial names.
        if (count($bio_repeats = array_diff_assoc($linepart, array_unique($linepart))) > 0) {
          //if (count($linepart) != count(array_unique($linepart))) {
          print "ERROR: There are multiple instance of the following " . "biomaterial(s) in the expression file:\n";
          foreach ($bio_repeats as $repeat) {
            print "  " . $repeat . "\n";
          }
          print "Please make sure there is only one instance of each " . "biomaterial in the expression file\n";
          exit;
        }

        $num_biomaterials = count($linepart);
        $mat_head = 1;
        continue;
      }
      else {

        // Make sure that there are enough expression columns.
        if ($num_col != ($num_biomaterials + 1)) {
          print "ERROR: Expression data in the data section of the matrix format must have a\n" . "feature name followed by tab separated expression values.\n" . "The number of expression values for each feature must equal\n" . "the number of biomaterials in the matrix file header.\n" . "Example matrix format:\n" . "  <feature name>       <expression value>	<expression value> ...\n";
          if (count($linepart) > 0) {
            print "Loader failed on the following line:\n" . "  " . $line;
            print "Loader expected " . ($num_biomaterials + 1) . " columns " . "but only found " . $num_col . " column(s).\n";
          }
          else {
            print "Loader failed because of a blank line in the data section of the expression file.\n";
          }
          exit;
        }

        //exit;

        if ($cur_feature % $int_length == 0) {
          $percent = sprintf("%.2f", ($cur_feature / $num_features) * 100);
          print "Loading complete: " . $percent . "%. Memory: " . number_format(memory_get_usage()) . " bytes. \n";
        }
        //  if ($j % $load_limit == 0) {
        //   print "Processing Feature $cur_feature of $num_features\n";
        // }
        $cur_feature++;
        for ($i = 1; $i < $num_col; $i++) {
          $feature_id = tripal_expression_find_feature_id($linepart[0], $organism_id, $feature_uniquenames);
          $element_id = tripal_expression_element($arraydesign_id, $feature_id);
          $elementresult_id = tripal_expression_elementresult($element_id, $quant_array[$i - 1], $linepart[$i]);
          // Create a record in the analysisfeature table. This will allow the analysis to appear in the 
          // analysis list on the feature page. 
          if ($i == 1) {
            $analysisfeature_id = chado_query('SELECT analysisfeature_id FROM {analysisfeature} WHERE analysis_id = :analysis_id AND feature_id = :feature_id', [
              ':analysis_id' => $analysis_id,
              ':feature_id' => $feature_id,
            ])->fetchField();
            if (!$analysisfeature_id) {
              $analysisfeature = chado_insert_record('analysisfeature', [
                'analysis_id' => $analysis_id,
                'feature_id' => $feature_id,
              ]);
              if ($analysisfeature == FALSE) {
                print "ERROR: Unable to write to analysisfeature table\n";
                exit;
              }
            }
            /*if($analysisfeature) {
              $analysisfeature = chado_update_record('analysisfeature', 
                array(':analysis_id' => $analysis_id, ':feature_id' => $feature_id),
                $values);
              if($analysisfeature==FALSE) {
                print "ERROR: Unable to update analysisfeature table\n";
                exit;
              }
            } */
          }
        }
      }
    }
    if (preg_match('/' . $re_start . '/', $line) and $re_start) {
      $data_ln = 1;
    }
  }
  fclose($mat_fp);
  return;
}

/**
 *
 * Implements the function that will parse an expression file in the column
 * format.
 *
 * @param $organism_id
 *  The organism_id of the organism from which the biomaterial was gathered.
 *
 * @param $analysis_id
 *  The analysis_id of the analysis: expression from which the expression
 *   loader is being called.
 *
 * @param $contact_id
 *  The contact_id of the contact that provided the biomaterial. This paramter
 *   is optional.
 *
 * @param $arraydesign_id
 *  The arraydesign_id of the arraydesign associated with the experiment. This
 *   parameter is optional.
 *
 * @param $assay_id
 *  The assay_id of the assay associated with the experiment. This parameter is
 *   optional.
 *
 * @param $acquisition_id
 *  The acquisition_id of the acquisition associated with the experiment. This
 *   parameter is optional.
 *
 * @param $quantificatin_id
 *  The quantification_id of the quantification associated with the experiment.
 *   This parameter is optional.
 *
 * @param $filepath
 *  The file path to the expression file/s to be loaded.
 *
 * @param $fileext
 *  The file extension of the expression files to be loaded.
 *
 * @ingroup tripal_analysis_expression
 */

function tripal_expression_load_col_file($filepath, $fileext, $arraydesign_id, $organism_id, $analysis_id, $contact_id, $assay_id, $acquisition_id, $quantification_id, $re_start, $re_stop, $uniq_name, $feature_uniquenames) {
  $inter = 0;
  $data_ln = 0;
  preg_match('/^.+\/(.+)\.' . $fileext . '$/', $filepath, $biomaterial);
  $quantification_id = expression_create_biomaterial_structure($biomaterial[1], $organism_id, $analysis_id, $contact_id, $arraydesign_id, $assay_id, $acquisition_id, $quantification_id, $uniq_name);

  $features = [];

  $col_head = 0;
  $num_features = 0;

  $col_fp = fopen($filepath, 'r');
  // Ignore the feature footer.
  while ($line = fgets($col_fp)) {
    $num_lines++;
    if (preg_match('/' . $re_stop . '/', $line) and $re_stop) {
      $data_ln = 0;
    }
    // Grab data lines. Ingore the header.
    if ($data_ln == 1 or !$re_start) {
      if ($col_head == 0) {
        $num_features++;
        $col_head = 1;
        continue;
      }
      elseif ($col_head == 1) {
        $num_features++;
        preg_match("/^(.+?)\t/", $line, $match);
        $feature_array[] = trim($match[1]);
      }
    }
    // Only capture lines after the header. 
    if (preg_match('/' . $re_start . '/', $line) and $re_start) {
      $data_ln = 1;
    }
  }

  fclose($col_fp);
  $data_ln = 0;

  // Make sure there not duplicate feature names.
  if (count($feature_repeats = array_diff_assoc($feature_array, array_unique($feature_array))) > 0) {
    //if (count($linepart) != count(array_unique($linepart))) {
    print "ERROR: There are multiple instance of the following " . "feature(s) in the expression file:\n";
    foreach ($feature_repeats as $repeat) {
      print "  " . $repeat . "\n";
    }
    print "Please make sure there is only one instance of each " . "feature in the expression file\n";
    exit;
  }

  // Get rid of the feature_array. 
  unset($feature_array);

  // Calculate interval length.
  $int_length = .01 * $num_features;

  // The minimum interval length is 1.
  if ($int_length < 1) {
    $int_length = 1;
  }

  $f_id = 0;
  $e_id = 0;
  $er_id = 0;
  $af_id = 0;

  print "Loading complete: " . 0 . "%. Memory: " . number_format(memory_get_usage()) . " bytes. \r";

  $col_fp = fopen($filepath, 'r');
  while ($line = fgets($col_fp)) {
    if (preg_match('/' . $re_stop . '/', $line) and $re_stop) {
      $data_ln = 0;
    }
    if ($data_ln == 1 or !$re_start) {
      //print "this is the line >>> $line <<<";
      $inter++;
      if ($inter % $int_length == 0) {
        $percent = sprintf("%.2f", ($inter / $num_features) * 100);
        print "Loading complete: " . $percent . "%. Memory: " . number_format(memory_get_usage()) . " bytes. \r";
        ob_end_flush();
        //  print "Find feature_id: " . $f_id/($num_features*.01) . "\n";
        //  print "Create element_id: " . $e_id/($num_features*.01) . "\n";
        //  print "Create elementresult_id: " . $er_id/($num_features*.01) . "\n";
        //  print "Create analysisfeature_id: " . $af_id/($num_features*.01) . "\n\n";

        $f_id = 0;
        $e_id = 0;
        $er_id = 0;
        $af_id = 0;
      }
      $linepart = preg_split('/\t/', $line, -1, PREG_SPLIT_NO_EMPTY);

      // Trim whitespace from columns.
      $new_linepart = [];
      foreach ($linepart as $col) {
        $new_linepart[] = trim($col);
      }
      $linepart = array_filter($new_linepart, function ($var) {
        return $var != "";
      });
      $linepart = array_values($linepart);

      // Make sure that there are two columns.
      if (count($linepart) != 2) {
        print "ERROR: Expression data in the data section of the column format must have two columns separated by a tab.\n" . "Example column format:\n" . "  <feature name>	<expression value>\n";
        if (count($linepart) > 0) {
          print "Loader failed on the following line:\n" . "  " . $line . "\n";
        }
        else {
          print "Loader failed because of a blank line in the data section of the expression file.\n";
        }
        exit;
      }

      $num_col = count($linepart);

      // Extract the feature id.
      $start = microtime(TRUE);
      $feature_id = tripal_expression_find_feature_id($linepart[0], $organism_id, $feature_uniquenames);

      // Add to feature list;
      $features[] = $feature_id;

      //print "Find feature_id: "; 
      $f_id += microtime(TRUE) - $start . "\n";

      $start = microtime(TRUE);
      $element_id = tripal_expression_element($arraydesign_id, $feature_id);
      //print "Create element_id: ";
      $e_id += microtime(TRUE) - $start . "\n";

      $start = microtime(TRUE);
      $elementresult_id = tripal_expression_elementresult($element_id, $quantification_id, $linepart[1]);
      //print "Create elementresult_id: "; 
      $er_id += microtime(TRUE) - $start . "\n";
    }
    if (preg_match('/' . $re_start . '/', $line) and $re_start) {
      $data_ln = 1;
    }
  }
  fclose($col_fp);
  return $features;
}

/**
 *
 * Implements a function that find the find the feature_id from a feature name.
 *
 * @param $feature
 *  The name of the feature to be found.
 *
 * @ingroup tripal_analysis_expression
 */

function tripal_expression_find_feature_id($feature, $organism_id, $feature_uniquenames) {

  if ($feature_uniquenames == 'uniq') {
    $sql = "SELECT feature_id, organism_id FROM {feature} WHERE uniquename = :feature";
  }
  else {
    $sql = "SELECT feature_id, organism_id FROM {feature} WHERE name = :feature";
  }

  $feature_object = chado_query($sql, [':feature' => $feature])->fetchObject();

  if (!$feature_object) {
    print "ERROR: The feature, " . $feature . ", found in the expression file " . "was not found in the Chado database. Please ensure that the " . "feature has been loaded into the database and that the feature name is " . "both unique and correct.\n\n";
    exit;
  }

  if ($feature_object->organism_id != $organism_id) {
    $sql = "SELECT common_name, genus, species FROM {organism} WHERE organism_id = :organism_id";
    $analysis_org = chado_query($sql, [":organism_id" => $organism_id])->fetchObject();
    $feature_org = chado_query($sql, [":organism_id" => $feature_object->organism_id])->fetchObject();
    print "ERROR: Mismatch between the organism selected in the expression analysis " . "and the organism of a feature from the expression file.\n" . "  Analysis organism: " . $analysis_org->genus . " " . $analysis_org->species . " (" . $analysis_org->common_name . ")\n" . "  Feature organism: " . $feature_org->genus . " " . $feature_org->species . " (" . $feature_org->common_name . ")\n" . "  On feature: " . $feature . "\n" . "Please ensure that the organism selected for the expression " . "analysis and the organism for each feature in the expression " . "file are the same.\n";
    exit;
  }

  return $feature_object->feature_id;
}
