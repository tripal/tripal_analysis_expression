<?php

/**
 * Implements the function that will called as a tripal job 
 *  to load expression data.
 *
 *  @param $organism_id
 *  The organism_id of the organism from which the biomaterial was gathered. 
 *
 *  @param $analysis_id
 *  The analysis_id of the analysis: expression from which the expression loader is being called.
 * 
 *  @param $contact_id
 *  The contact_id of the contact that provided the biomaterial. This paramter is optional.
 * 
 *  @param $arraydesign_id
 *  The arraydesign_id of the arraydesign associated with the experiment. This parameter is optional.
 * 
 *  @param $assay_id
 *  The assay_id of the assay associated with the experiment. This parameter is optional. 
 * 
 *  @param $acquisition_id
 *  The acquisition_id of the acquisition associated with the experiment. This parameter is optional.
 * 
 *  @param $quantificatin_id
 *  The quantification_id of the quantification associated with the experiment. This parameter is optional.
 *  
 *  @param $filepath
 *  The file path to the expression file/s to be loaded. 
 *
 *  @param $fileext
 *  The file extension of the expression files to be loaded.
 * 
 *  @param $filetype
 *  The file type can either by mat for matrix or col for column.
 *
 * @ingroup tripal_analysis_expression
 */

function tripal_expression_loader($organism_id, $analysis_id, $contact_id, $arraydesign_id, $assaydate,
  $acquisitiondate, $quantificationdate, $filepath, $fileext, $filetype, $re_start, $re_stop) {

  $uniq_name = get_unique_name($organism_id, $analysis_id);

  $transaction = db_transaction();
  print "\nNOTE: Loading of this expression file is performed using a database transaction. \n" . 
        "If the load fails or is terminated prematurely then the entire set of \n" . 
        "insertions/updates is rolled back and will not be found in the database\n\n";
  try {
    if(!$contact_id) {
      $contact_id = create_generic_contact(); //***
    }
    // Create the array design. 
    $arraydesign_id   = $arraydesign_id    ? $arraydesign_id    : create_generic_arraydesign($contact_id);
    $re_ext = '/^.+\.(' . $fileext . ')/';

    // A matrix file.
    if ($filetype == 'mat' and is_file($filepath) and preg_match($re_ext,$filepath)) {
      tripal_expression_load_mat_file($filepath, $fileext, $arraydesign_id, $organism_id, $analysis_id, 
        $contact_id, $assay_id, $acquisition_id, $quantification_id, $re_start, $re_stop, $uniq_name); 
    }

    // The column format.
    elseif ($filetype == 'col' and is_file($filepath) and preg_match($re_ext,$filepath)) {
      tripal_expression_load_col_file($filepath, $fileext, $arraydesign_id, $organism_id, $analysis_id, 
        $contact_id, $assay_id, $acquisition_id, $quantification_id, $re_start, $re_stop, $uniq_name); 

    } else { // Multiple files in the column format.
      $dir_handle = @opendir($filepath) or die("Unable to open $filepath");
      $total_files = count(glob($filepath . '/*.' . $fileext));
      $num_file = 0;
      
      // Load all files with of type $fileext in the directory.
      while ($file = readdir($dir_handle)) {
        if (preg_match('/(.+)\.' . $fileext . '/',$file)) {
          print "File $num_file of $total_files: $file			\n";
          tripal_expression_load_col_file($filepath . '/' . $file, $fileext, $arraydesign_id, $organism_id, $analysis_id, 
            $contact_id, $assay_id, $acquisition_id, $quantification_id, $re_start, $re_stop, $uniq_name);
          $num_file++;
	}
      }
    }
  }
  catch (Exception $e) {
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
  $uniq_string .= chado_query($sql, array(':organism_id' => $organism_id))->fetchField();
  $uniq_string .= " for ";
  $sql = "SELECT name FROM {analysis} WHERE analysis_id = :analysis_id";
  $uniq_string .= chado_query($sql, array(':analysis_id' => $analysis_id))->fetchField();
 print $uniq_string; 
  return $uniq_string; 
}




/**
 * 
 * Implements the function that will parse an expression file with a matrix file. 
 * 
 *  @param $organism_id
 *  The organism_id of the organism from which the biomaterial was gathered. 
 *
 *  @param $analysis_id
 *  The analysis_id of the analysis: expression from which the expression loader is being called.
 * 
 *  @param $contact_id
 *  The contact_id of the contact that provided the biomaterial. This paramter is optional.
 * 
 *  @param $arraydesign_id
 *  The arraydesign_id of the arraydesign associated with the experiment. This parameter is optional.
 * 
 *  @param $assay_id
 *  The assay_id of the assay associated with the experiment. This parameter is optional. 
 * 
 *  @param $acquisition_id
 *  The acquisition_id of the acquisition associated with the experiment. This parameter is optional.
 * 
 *  @param $quantificatin_id
 *  The quantification_id of the quantification associated with the experiment. This parameter is optional.
 *  
 *  @param $filepath
 *  The file path to the expression file/s to be loaded. 
 *
 *  @param $fileext
 *  The file extension of the expression files to be loaded.
 * 
 * @ingroup tripal_analysis_expression
 */

function tripal_expression_load_mat_file($filepath, $fileext, $arraydesign_id, $organism_id, $analysis_id,
  $contact_id, $assay_id, $acquisition_id, $quantification_id, $re_start, $re_stop, $uniq_name) {
  
  $data_ln = 0;
  $mat_fp = fopen($filepath,'r');
  $num_col = 0;
  $mat_head = 0;
  
  $quant_array = array();

  // Loop through every line in a file.
  while($line = fgets($mat_fp)) {
    if (preg_match('/' . $re_stop . '/',$line) and $re_stop ) {
      $data_ln = 0; 
    }    
    if ($data_ln == 1 or !$re_start) {
      $linepart = preg_split('/\s+/',$line,-1,PREG_SPLIT_NO_EMPTY);
      $num_col = count($linepart);
  
      if ($mat_head == 0) {
        foreach ($linepart as $sample) {
          $quant_array[] = expression_create_biomaterial_structure($sample, $organism_id, $analysis_id, $contact_id, $arraydesign_id, $assay_id, $acquisition_id, $quantification_id, $uniq_name);
        }
        $mat_head = 1; 
      } else {
        for ($i = 1; $i < $num_col; $i++) {
          $feature_id = tripal_expression_find_feature_id($linepart[0]);
          $element_id = tripal_expression_element($arraydesign_id, $feature_id); 
          $elementresult_id = tripal_expression_elementresult($element_id, $quant_array[$i-1], $linepart[$i]);
          // Create a record in the analysisfeature table. This will allow the analysis to appear in the 
          // analysis list on the feature page. 
          if ($i == 1) {
            $analysisfeature_id = chado_query('SELECT analysisfeature_id FROM {analysisfeature} WHERE analysis_id = :analysis_id AND feature_id = :feature_id', array(':analysis_id' => $analysis_id, ':feature_id' => $feature_id))->fetchField();
            if(!$analysisfeature_id) {
              $analysisfeature = chado_insert_record('analysisfeature', array('analysis_id' => $analysis_id, 'feature_id' => $feature_id));
              if($analysisfeature==FALSE) {
                print "Unable to write to analysisfeature table\n";
                return 0;
              }
            }
            if($analysisfeature) {
              $analysisfeature = chado_update_record('analysisfeature', 
                array(':analysis_id' => $analysis_id, ':feature_id' => $feature_id),
                $values);
              if($analysisfeature==FALSE) {
                print "Unable to update analysisfeature table\n";
                return 0;
              }
            }
          }
        }    
      } 
    }
    if (preg_match('/' . $re_start . '/',$line) and $re_start) {
      $data_ln = 1; 
    }    
  }
  fclose($mat_fp);
  return;
}

/**
 * 
 * Implements the function that will parse an expression file in the column format.
 *
 *  @param $organism_id
 *  The organism_id of the organism from which the biomaterial was gathered. 
 *
 *  @param $analysis_id
 *  The analysis_id of the analysis: expression from which the expression loader is being called.
 * 
 *  @param $contact_id
 *  The contact_id of the contact that provided the biomaterial. This paramter is optional.
 * 
 *  @param $arraydesign_id
 *  The arraydesign_id of the arraydesign associated with the experiment. This parameter is optional.
 * 
 *  @param $assay_id
 *  The assay_id of the assay associated with the experiment. This parameter is optional. 
 * 
 *  @param $acquisition_id
 *  The acquisition_id of the acquisition associated with the experiment. This parameter is optional.
 * 
 *  @param $quantificatin_id
 *  The quantification_id of the quantification associated with the experiment. This parameter is optional.
 *  
 *  @param $filepath
 *  The file path to the expression file/s to be loaded. 
 *
 *  @param $fileext
 *  The file extension of the expression files to be loaded.
 * 
 * @ingroup tripal_analysis_expression
 */

function tripal_expression_load_col_file($filepath, $fileext, $arraydesign_id, $organism_id, $analysis_id,
  $contact_id, $assay_id, $acquisition_id, $quantification_id, $re_start, $re_stop, $uniq_name) {
  $inter = 0; 
  $data_ln = 0;
  preg_match('/^.+\/(.+)\.' . $fileext . '/', $filepath, $biomaterial);
  $quantification_id = expression_create_biomaterial_structure($biomaterial[1], $organism_id, 
    $analysis_id, $contact_id, $arraydesign_id, $assay_id, $acquisition_id, $quantification_id, $uniq_name);
  
  $col_fp = fopen($filepath,'r');
  while($line = fgets($col_fp)) {
    if (preg_match('/' . $re_stop . '/',$line) and $re_stop ) {
      $data_ln = 0; 
    }    
    if ($data_ln == 1 or !$re_start) {
      //print "this is the line >>> $line <<<";
      $inter++;
      print "loading number: $inter\n";
      $linepart = preg_split('/\s+/',$line,-1,PREG_SPLIT_NO_EMPTY);
      // Extract the feature id.
      $feature_id = tripal_expression_find_feature_id($linepart[0]);
      $element_id = tripal_expression_element($arraydesign_id, $feature_id);
      $elementresult_id = tripal_expression_elementresult($element_id, $quantification_id, $linepart[1]);
      // Create a record in the analysisfeature table. This will allow the analysis to appear in the 
      // analysis list on the feature page. 
      //chado_insert_record('analysisfeature', array('analysis_id' => $analysis_id, 'feature_id' => $feature_id));
      $analysisfeature_id = chado_query('SELECT analysisfeature_id FROM {analysisfeature} WHERE analysis_id = :analysis_id AND feature_id = :feature_id', array(':analysis_id' => $analysis_id, ':feature_id' => $feature_id))->fetchField();
      if(!$analysisfeature_id) {
        $analysisfeature = chado_insert_record('analysisfeature', array('analysis_id' => $analysis_id, 'feature_id' => $feature_id));
        if($analysisfeature==FALSE) {
          print "Unable to write to analysisfeature table\n";
          return 0;
        }
      }
      if($analysisfeature) {
        $analysisfeature = chado_update_record('analysisfeature',
          array(':analysis_id' => $analysis_id, ':feature_id' => $feature_id),
          $values);
        if($analysisfeature==FALSE) {
          print "Unable to update analysisfeature table\n";
          return 0;
        }
      }
    }
    if (preg_match('/' . $re_start . '/',$line) and $re_start) {
      $data_ln = 1; 
    }    
  }
  fclose($col_fp);
  return;
}

/**
 * 
 * Implements a function that find the find the feature_id from a feature name.
 * 
 *  @param $feature
 *  The name of the feature to be found.
 * 
 * @ingroup tripal_analysis_expression
 */

function tripal_expression_find_feature_id($feature) {
  $sql = "SELECT feature_id FROM {feature} WHERE name = :feature";
  return chado_query($sql, array(':feature' => $feature))->fetchField();
}

/**
 * 
 * Implements a function that inserts a record into the element table.
 *
 *  @param $arraydesign_id
 *  The arraydesign_id of the arraydesign associated with the experiment. This parameter 
 *   is necessary for the creation of the element record. 
 * 
 *  @param $feature_id
 *  The feature_id of the feature associated with the element. This parameter
 *   is necessary for the creation of the element record. 
 *
 * @ingroup tripal_analysis_expression
 */

function tripal_expression_element($arraydesign_id, $feature_id) {
  $element_id = chado_query('SELECT element_id FROM {element} WHERE arraydesign_id = :arraydesign_id AND feature_id = :feature_id', array(':arraydesign_id' => $arraydesign_id, ':feature_id' => $feature_id))->fetchField();
  if(!$element_id) {
    $values = array(
      'arraydesign_id' => $arraydesign_id,
      'feature_id' => $feature_id,
    );
    $element = chado_insert_record('element',$values);
    if($element==FALSE) {
      print "Unable to write to element table\n";
      return 0;
    }
    $element_id = $element['element_id'];
  }
  return $element_id;
}

/**
 *
 * Implements a funtion that inserts a record into the elementresult table.
 * 
 *  @param $element_id
 *  The element_id of the element associated with the elementresult. This parameter
 *   is necessary for the creation of the elementresult record. 
 * 
 *  @param $quantification_id
 *  The quantification_id of the quantification associated with the experiment. This parameter
 *   is necessary for the creation of the elementresult record. 
 *  
 * @ingroup tripal_analysis_expression
 */

function tripal_expression_elementresult($element_id, $quantification_id, $signal) {

  $values = array(
    'element_id' => $element_id,
    'quantification_id' => $quantification_id,
    'signal' => $signal,
  );
 
  $elementresult_id = chado_query('SELECT elementresult_id FROM {elementresult} WHERE element_id = :element_id AND quantification_id = :quantification_id', array(':element_id' => $element_id, ':quantification_id' => $quantification_id))->fetchField();
  if(!$elementresult_id) {
    $elementresult = chado_insert_record('elementresult',$values);
    if($elementresult==FALSE) {
      print "Unable to write to elementresult table\n";
      return 0;
    }
  }
  if($elementresult_id) {
    $elementresult = chado_update_record('elementresult', 
      array('element_id' => $element_id, 'quantification_id' => $quantification_id), 
      $values);
    if($elementresult==FALSE) {
      print "Unable to update elementresult table\n";
      return 0;
    }
  }
    $elementresult_id = $elementresult['elementresult_id'];
    return $elementresult_id;
}

/**
 *
 * Implements a function to create records in the following tables: biomaterial,
 *  contact, assay, acquisition, quantification, channel, and assay_biomaterial.
 *  The function will return a quantification_id that can be associated with
 *  features through the element and elementresult tables. 
 *
 *  @param $biomaterial
 *  The name of a biomaterial. This name will be used to create a new biomaterial record.  
 * 
 *  @param $organism_id
 *  The organism_id of the organism from which the biomaterial was gathered. 
 *
 *  @param $analysis_id
 *  The analysis_id of the analysis: expression from which the expression loader is being called.
 * 
 *  @param $contact_id
 *  The contact_id of the contact that provided the biomaterial. This paramter is optional.
 *  If the parameter is not provided, a generic contact record will be created in the contact table.
 * 
 *  @param $arraydesign_id
 *  The arraydesign_id of the arraydesign associated with the experiment.
 * 
 *  @param $assay_id
 *  The assay_id of the assay associated with the experiment. This parameter is optional. 
 *  If the parameter is not provided, a generic assay record will be created in the assay table.
 * 
 *  @param $acquisition_id
 *  The acquisition_id of the acquisition associated with the experiment. This parameter is optional.
 *  If the parameter is not provided, a generic acquisition record will be created in the acquisition table.
 * 
 *  @param $quantificatin_id
 *  The quantification_id of the quantification associated with the experiment. This parameter is optional.
 *  If the parameter is not provided, a generic quantification record will be created in the quantification table.
 *  
 * @ingroup tripal_analysis_expression
 */

function expression_create_biomaterial_structure($biomaterial, $organism_id, $analysis_id, $contact_id, $arraydesign_id, $assay_id, $acquisition_id, $quantification_id, $uniq_name) {
  // Create the biomaterial
  $biomaterial_id = create_biomaterial($biomaterial, $analysis_id, $organism_id, $contact_id, $dbxref_id, '');

  $uniq_name = $biomaterial . $uniq_name;

  // Create the following records if not provided by the user. 
  $contact_id       = $contact_id        ? $contact_id        : create_generic_contact();
  $assay_id = create_generic_assay($contact_id, $arraydesign_id, $biomaterial_id, $organism_id, $analysis_id, $uniq_name);
  $acquisition_id = create_generic_acquisition($assay_id, $biomaterial_id, $organism_id, $analysis_id, $uniq_name);
  $quantification_id = create_generic_quantification($acquisition_id, $biomaterial_id, $organism_id, $analysis_id, $uniq_name);
  //$assay_id         = $assay_id          ? create_unique_assay($assay_id, $contact_id, $arraydesign_id, $biomaterial_id, $organism_id, $analysis_id) 
   // : create_generic_assay($contact_id, $arraydesign_id, $biomaterial_id, $organism_id, $analysis_id);
  //$acquisition_id   = $acquisition_id    ? create_unique_acquisition($acquisition_id, $assay_id, $biomaterial_id, $organism_id, $analysis_id)        
  //  : create_generic_acquisition($assay_id, $biomaterial_id, $organism_id, $analysis_id);
 // $quantification_id = $quantification_id ? create_unique_quantification($quantification_id, $acquisition_id, $biomaterial_id, $organism_id, $analysis_id) 
   // : create_generic_quantification($acquisition_id, $biomaterial_id, $organism_id, $analysis_id);

  // Create a generic channel. A channel_id is necessary for the assay_biomaterial table. 
  $channel_id = create_generic_channel();
  $assay_biomaterial_id = create_assay_biomaterial($assay_id, $biomaterial_id, $channel_id);
    
  return $quantification_id;
}

/**
 * 
 * Implements a function to create a record in the assay_biomaterial table. The assay_biomaterial
 *  table connects the assay and biomaterial table. As well as an assay and biomaterial record, the 
 *  assay_biomaterial table also requires a channel record.
 * 
 *  @param $assay_id
 *  The assay_id of the assay associated with the experiment. This parameter is necessary for the creation
 *  of an assay_biomaterial record. 
 *
 *  @param $biomaterial_id
 *  The biomaterial_id of one of the biomaterials in the experiment. This parameter is necessary
 *  for the creation of the assay_biomaterial record.
 * 
 *  @param $channel_id
 *  The channel_id of the channel of the assay containing the biomaterial. This parameter is necessary 
 *  for the creation of the assay_biomaterial record. 
 *  
 * @ingroup tripal_analysis_expression
 */
 
function create_assay_biomaterial($assay_id, $biomaterial_id, $channel_id) {
  $assay_biomaterial_id = chado_query('select assay_biomaterial_id from {assay_biomaterial} where rank = :rank and assay_id = :assay_id and biomaterial_id = :biomaterial_id and channel_id = :channel_id limit 1',array(':rank' => 1, ':assay_id' => $assay_id, ':biomaterial_id' => $biomaterial_id, ':channel_id' => $channel_id))->fetchField();
  if(!$assay_biomaterial_id) {
    $values = array(
      'rank' => 1,
      'assay_id' => $assay_id,
      'biomaterial_id' => $biomaterial_id,
      'channel_id' => $channel_id,
    );
    $assay_biomaterial = chado_insert_record('assay_biomaterial', $values);
    if($assay_biomaterial==FALSE) {
      print "Unable to write to assay_biomaterial table\n";
      return 0;
    }
    $assay_biomaterial_id = $assay_biomaterial['assay_biomaterial_id'];
  }
  return $assay_biomaterial_id;
}

/**
 *
 * Implements a function to create a generic contact.
 * 
 * @ingroup tripal_analysis_expression
 */

function create_generic_contact() {
  $contact_id = chado_query('select contact_id from {contact} where name = :name limit 1',array(':name' => 'generic contact'))->fetchField();
  if(!$contact_id) {
    $values = array(
      'name' => 'generic contact',
      'description' => 'Caution: This is a generic contact created by the expression module. This contact may be a dependency for an expression analysis.',
    );
    $contact = chado_insert_record('contact', $values);
    if($contact==FALSE) {
      print "Unable to write to contact table\n";
      return 0;
    }
    $contact_id = $contact['contact_id'];
  }
  return $contact_id;
}

/**
 *
 * Implements a function to create a generic channel.
 * 
 * @ingroup tripal_analysis_expression
 */

function create_generic_channel() {
  $channel_id = chado_query('select channel_id from {channel} where name = :name limit 1',array(':name' => 'generic channel'))->fetchField();
  if(!$channel_id) {
    $values = array(
      'name' => 'generic channel',
      'definition' => 'Caution: This is a generic channel created by the expression module. This channel may be a dependency for an expression analysis.',
    );
    $channel = chado_insert_record('channel',$values);
    if($channel==FALSE) {
      print "Unable to write to channel table\n";
      return 0;
    }
    $channel_id = $channel['channel_id'];
  }
  return $channel_id;
}

/**
 *
 * Implements a function to create a generic arraydesign.
 * 
 * @ingroup tripal_analysis_expression
 */

function create_generic_arraydesign($contact_id) {
  $arraydesign_id = chado_query('select arraydesign_id from {arraydesign} where name = :name limit 1',array(':name' => 'generic arraydesign'))->fetchField(); 
  if(!$arraydesign_id) {
    $values = array(
      'name' => 'generic arraydesign',
      'description' => 'Caution: This is a generic arraydesign created by the expression module. This arraydesign may be a dependency for an expression analysis.',
      'manufacturer_id' => $contact_id,
      'platformtype_id' => 1,  
    );
    $arraydesign = chado_insert_record('arraydesign',$values);
    if($arraydesign==FALSE) {
      print "Unable to write to arraydesign table\n";
      return 0;
    }
    $arraydesign_id = $arraydesign['arraydesign_id'];
  }
  return $arraydesign_id;
}

function create_unique_arraydesign($arraydesign_id, $organism_id, $analysis_id) {
  $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $organism_s  = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id', array(':analysis_id' => $analysis_id))->fetchField();

  // Grab every field except the foreign key. 
  $columns = array('name','version','description', 'array_dimensions', 'element_dimensions', 
    'num_of_elements', 'num_array_columns', 'num_array_rows', 'num_grid_columns',
    'num_grid_rows', 'num_sub_columns', 'num_sub_rows', 'manufacturer_id', 'platformtype_id', 
    'platformtype_id', 'dbxref_id', 'dbxref_id');

  $select_values = array('arraydesign_id' => $arraydesign_id); 
  $values = chado_select_record('arraydesign', $columns, $select_values);
  $values = (array)$values;
  
  // Update the name field to create a unique record. 
  $values['name'] = $values['name'] . ' - Expression analysis: ' . $analysis . ' of organism: ' . $organism_g . ' ' . $organism_s;
  
  $arraydesign = chado_insert_record('arraydesign',$values);  
  $arraydesign_id = $arraydesign['arraydesign_id'];
  return $arraydesign_id;
}

function create_unique_assay($assay_id, $contact_id, $arraydesign_id, $biomaterial_id, $organism_id, $analysis_id) {
  $biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id', array(':biomaterial_id' => $biomaterial_id))->fetchField();
  $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $organism_s  = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id', array(':analysis_id' => $analysis_id))->fetchField();

  // Grab every field except the foreign key. 
  $columns = array('assaydate', 'arrayidentifier', 'arraybatchidentifier', 'name', 'description', 
    'arraydesign_id', 'operator_id', 'dbxref_id', 'protocol_id');

  $select_values = array('assay_id' => $assay_id); 
  $values = chado_select_record('assay', $columns, $select_values);
  $values = (array)$values;

  // Update the name field to create a unique record. 
  $values['name'] = $values['name'] . ' - ' . $biomaterial . ' - ' . $organism_g . ' ' . $organism_s . ' - ' . $analysis;

  $assay = chado_insert_record('assay',$values);  
  $assay_id = $assay['assay_id'];
  return $assay_id;
}
 
function create_unique_acquisition($acquisition_id, $assay_id, $biomaterial_id, $organism_id, $analysis_id) {
  $biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id', array(':biomaterial_id' => $biomaterial_id))->fetchField();
  $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $organism_s  = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id', array(':analysis_id' => $analysis_id))->fetchField();

  // Grab every field except the foreign key. 
  $columns = array('acquisitiondate', 'name', 'uri', 'assay_id', 'channel_id', 'protocol_id');

  $select_values = array('acquisition_id' => $acquisition_id); 
  $values = chado_select_record('acquisition', $columns, $select_values);
  $values = (array)$values;

  // Update the name field to create a unique record. 
  $values['name'] = $values['name'] . ' - ' . $biomaterial . ' - ' . $organism_g . ' ' . $organism_s . ' - ' . $analysis;
  $values['assay_id'] = $assay_id;

  $acquisition = chado_insert_record('acquisition',$values);  
  $acquisition_id = $acquisition['acquisition_id'];
  return $acquisition_id;
}

function create_unique_quantification($quantification_id, $acquisition_id, $biomaterial_id, $organism_id, $analysis_id) {
  $biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id', array(':biomaterial_id' => $biomaterial_id))->fetchField();
  $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $organism_s  = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id', array(':analysis_id' => $analysis_id))->fetchField();

  // Grab every field except the foreign key. 
  $columns = array('quantificationdate', 'name', 'uri', 'acquisition_id', 'analysis_id', 'operator_id', 'protocol_id');

  $select_values = array('quantification_id' => $quantification_id); 
  $values = chado_select_record('quantification', $columns, $select_values);
  $values = (array)$values;

  // Update the name field to create a unique record. 
  $values['name'] = $values['name'] . ' - ' . $biomaterial . ' - ' . $organism_g . ' ' . $organism_s . ' - ' . $analysis;
  $values['acquisition_id'] = $acquisition_id;

  $quantification = chado_insert_record('quantification',$values);  
  $quantification_id = $quantification['quantification_id'];
  return $quantification_id;
}

/**
/**
 *
 * Implements a function to create a generic assay. Only a name, an arraydesign_id, 
 *  and a contact_id are required to create an assay record. A biomaterial_id, 
 *  organism_id, and analysis_id are provided to the function to create a unique
 *  assay name. 
 *
 *  @param $contact_id
 *  The contact_id of the operator associated with the assay. In the case of this
 *  function this contact_id will belong to a generic contact record. This parameter
 *  is necessary for the creation of the assay record. 
 *
 *  @param $arraydesign
 *  The arraydesign_id of the array design associated with the assay. This parameter
 *  is necessary for the creation of the assay record. 
 * 
 *  @param $biomaterial_id
 *  The biomaterial_id of one of the biomaterials of the experiment. This will be used
 *  to create a unique name for the assay. 
 *  
 *  @param $organism_id
 *  The organism_id of the organism associated with the experiment. This will be used
 *  to create a unique name for the assay. 
 *  
 *  @param $analysis_id
 *  The analysis_id of the analysis associated with the experiment. This will be used
 *  to create a unique name for the assay. 
 * 
 * @ingroup tripal_analysis_expression
 */

function create_generic_assay($contact_id, $arraydesign_id, $biomaterial_id, $organism_id, $analysis_id, $uniq_name) {
  /*$biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id', array(':biomaterial_id' => $biomaterial_id))->fetchField();
  $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $organism_s  = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id', array(':analysis_id' => $analysis_id))->fetchField(); */
  $assay_id = chado_query('select assay_id from {assay} where name = :name limit 1',array(':name' => $uniq_name))->fetchField(); 
  //$assay_id = chado_query('select assay_id from {assay} where name = :name limit 1',array(':name' => $biomaterial . ' - ' . $organism_g . " " . $organism_s . ' - ' . $analysis))->fetchField(); 
  if(!$assay_id) {
    $values = array(
      'name' => $uniq_name,
      //'name' => $biomaterial . ' - ' . $organism_g . " " . $organism_s . ' - ' . $analysis,
      //'description' => 'This is a generic assay created for the biomaterial: '. $biomaterial . '. The biomaterial was taken from the organism: ' . $organism_g . ' ' . $organism_s . '. This assay is part of the analysis: ' . $analysis,
      'arraydesign_id' => $arraydesign_id,
      'operator_id' => $contact_id, ####**** 
    );
    $assay = chado_insert_record('assay',$values);
    if($assay==FALSE) {
      print "Unable to write to assay table\n";
      return 0;
    }
    $assay_id = $assay['assay_id'];
  }
  return $assay_id;
}
  
/**
 *
 * Implements a function to create a biomaterial. The name field in the biomaterial, 
 *  is the only field that is required to create a biomaterial record. However, becuase this
 *  is designed to be the primary method by biomaterials are loaded into the table, the function 
 *  provides extra parameters with which to describe the biomaterial. 
 *
 *  @param $biomaterial
 *  The name of the biomaterial for which a record will be created. 
 * 
 *  @param $analysis_id 
 *  The analysis_id of the analysis associated with the experiment. 
 * 
 *  @param $organism_id
 *  The organism_id of the organism associated with the experiment. 
 * 
 *  @param $biosourceprovider_id
 *  The contact_id of the contact who provided the biomaterial.
 * 
 * @ingroup tripal_analysis_expression
 */ 

function create_biomaterial($biomaterial, $analysis_id, $organism_id, $biosourceprovider_id, $dbxref_id, $biomaterial_description) {
  $biomaterial_id = chado_query('select biomaterial_id from {biomaterial} where name = :name limit 1',array(':name' => $biomaterial))->fetchField();

drupal_set_message($biomaterial_description);
    if ($biomaterial_description == '') {
      $biomaterial_description = 'This biomaterial: ' . $biomaterial . ', was created for the analysis: ' . $analysis_id;
    }
drupal_set_message($biomaterial_description);

    $values = array(
      'name' => $biomaterial,
      'description' => $biomaterial_description,
      //'biosourceprovider_id' => $biosourceprovider_id,
      'taxon_id' => $organism_id, 
      'dbxref_id' => $dbxref_id, 
    );

  if($biosourceprovider_id) {
    $values['biosourceprovider_id'] = $biosourceprovider_id;
  }

  if(!$biomaterial_id) {
    $biomaterial = chado_insert_record('biomaterial',$values);
    if($biomaterial==FALSE) {
      print "Unable to write to biomaterial table\n";
      return 0;
    }
  }
  if($biomaterial_id) {
    $biomaterial = chado_update_record('biomaterial',
      array('name' => $biomaterial),
      $values,
      array('return_record' => TRUE));
    if($biomaterial==FALSE) {
      print "Unable to update biomaterial_table\n";
      return 0;
    }
  } 
  $biomaterial_id = $biomaterial['biomaterial_id'];

  return $biomaterial_id;
}

/**
 *
 * Implements a function to create a generic acquisition. Only a name and assay_id, 
 *  are required to create an assay record. A biomaterial_id, organism_id, and 
 *  analysis_id are provided to the function to create a unique acquisition name. 
 *
 *  @param $assay
 *  The assay_id of the assay with which the acquisition is associated. 
 *  This parameter is necessary for the creation of the acquisition record. 
 * 
 *  @param $biomaterial_id
 *  The biomaterial_id of one of the biomaterials of the experiment. This will be used
 *  to create a unique name for the acquisition. 
 *  
 *  @param $organism_id
 *  The organism_id of the organism associated with the experiment. This will be used
 *  to create a unique name for the acquisition. 
 *  
 *  @param $analysis_id
 *  The analysis_id of the analysis associated with the experiment. This will be used
 *  to create a unique name for the acquisition. 
 * 
 * @ingroup tripal_analysis_expression
 */

function create_generic_acquisition($assay_id, $biomaterial_id, $organism_id, $analysis_id, $uniq_name) {
 /* $biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id', array(':biomaterial_id' => $biomaterial_id))->fetchField();
  $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $organism_s  = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id', array(':analysis_id' => $analysis_id))->fetchField();*/
  $acquisition_id = chado_query('select acquisition_id from {acquisition} where name = :name limit 1',array(':name' => $uniq_name))->fetchField(); 
  //$acquisition_id = chado_query('select acquisition_id from {acquisition} where name = :name limit 1',array(':name' => $biomaterial . ' - ' . $organism_g . " " . $organism_s . ' - ' . $analysis))->fetchField(); 
  if(!$acquisition_id) {
    $values = array(
      'name' => $uniq_name, //biomaterial . ' - ' . $organism_g . " " . $organism_s . ' - ' . $analysis,
      //'name' => $biomaterial . ' - ' . $organism_g . " " . $organism_s . ' - ' . $analysis,
      'assay_id' => $assay_id,
    );
    $acquisition = chado_insert_record('acquisition',$values);
    if($acquisition==FALSE) {
      print "Unable to write to acquisition table\n";
      return 0;
    }
    $acquisition_id = $acquisition['acquisition_id'];
  }
  return $acquisition_id;
}

/**
 *
 * Implements a function to create a generic quantification. Only a name, acuqisition_id, 
 *  and analysis_id are required to create an assay record. A biomaterial_id, organism_id, 
 *  and analysis_id are provided to the function to create a unique quantification name. 
 *
 *  @param $acquisition
 *  The acquisition_id of the acquisition with which the quantification is associated. 
 *  This parameter is necessary for the creation of the quantification record. 
 * 
 *  @param $biomaterial_id
 *  The biomaterial_id of one of the biomaterials of the experiment. This will be used
 *  to create a unique name for the quantification. 
 *  
 *  @param $organism_id
 *  The organism_id of the organism associated with the experiment. This will be used
 *  to create a unique name for the quantification. 
 *  
 *  @param $analysis_id
 *  The analysis_id of the analysis associated with the experiment. This parameter is 
 *  necessary for the creation of the quantification record. This record will also be used
 *  to create a unique name for the quantification. 
 *
 * @ingroup tripal_analysis_expression
 */
  
function create_generic_quantification($acquisition_id, $biomaterial_id, $organism_id, $analysis_id, $uniq_name) {
  /*$biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id', array(':biomaterial_id' => $biomaterial_id))->fetchField();
  $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $organism_s  = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id', array(':analysis_id' => $analysis_id))->fetchField(); */
  $quantification_id = chado_query('select quantification_id from {quantification} where name = :name limit 1',array(':name' => $uniq_name))->fetchField(); 
  //$quantification_id = chado_query('select quantification_id from {quantification} where name = :name limit 1',array(':name' => $biomaterial . ' - ' . $organism_g . " " . $organism_s . ' - ' . $analysis))->fetchField(); 
  if(!$quantification_id) {
    $values = array(
      'name' => $uniq_name,
      //'name' => $biomaterial . ' - ' . $organism_g . " " . $organism_s . ' - ' . $analysis,
      'acquisition_id' => $acquisition_id,
      'analysis_id' => $analysis_id,
    );
    $quantification = chado_insert_record('quantification',$values);
    if($quantification==FALSE) {
      print "Unable to write to quantification table\n";
      return 0;
    }
    $quantification_id =  $quantification['quantification_id'];
  }
  return $quantification_id;
}

