<?php

function feature_heatmap_form($form, &$form_state){
  $form['descriptions'] = array(
    '#type' => 'item',
    '#markup' => t('<b>Feature unique names need to be separated by comma(,).</b>')
  );

  $form['heatmap_feature_uniquename'] = array(
    '#type' => 'textarea',
    '#title' => t('Enter feature unique names'),
    '#description' => t('<b>Example feature unique names:</b> Juglans_nigra_120313_comp34229_c0_seq1 ,Quercus_rubra_120313_comp31392_c0_seq1, Acer_saccharum_022416_comp57075_c0_seq8, Liquidambar_styraciflua_01052015_comp34573_c0_seq2,Fraxinus_pennsylvanica_120313_comp50845_c0_seq2, Nyssa_sylvatica_10132014_comp27824_c0_seq1, Quercus_rubra_120313_comp13209_c0_seq1'),
    '#attributes' => array(
      'placeholder' => t('Nyssa_sylvatica_10132014_comp23154_c0_seq12_m.8255, Quercus_rubra_120313_comp172643_c0_seq1_m.37896, Liriodendron_tulipifera_10132014_comp16064_c0_seq1'),
    ),
  );


  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Display Expression Heatmap'),
  );

  return $form;
}


/*
 * feature_heatmap_form submit handler
 */
function feature_heatmap_form_submit($form, &$form_state){

  $sql_feature_uniquename = "SELECT ER.elementresult_id, ER.signal, F.feature_id, F.uniquename, 
                                    Q.name AS quantification_name, Q.analysis_id, 
                                    B.name, B.biomaterial_id, B.description FROM {chado.elementresult} ER
           INNER JOIN {chado.element} E ON E.element_id = ER.element_id
           INNER JOIN {chado.feature} F ON F.feature_id = E.feature_id
           INNER JOIN {chado.quantification} Q ON Q.quantification_id = ER.quantification_id
           INNER JOIN {chado.acquisition} AQ ON AQ.acquisition_id = Q.acquisition_id
           INNER JOIN {chado.assay} A ON A.assay_id = AQ.assay_id
           INNER JOIN {chado.assay_biomaterial} AB ON AB.assay_id = A.assay_id
           INNER JOIN {chado.biomaterial} B ON B.biomaterial_id = AB.biomaterial_id
         WHERE
           F.uniquename = :feature_uniquename";


  $expressions = array();
  $heatmap_feature_uniquename = $form_state['values']['heatmap_feature_uniquename'];


  if(!empty($heatmap_feature_uniquename)) {  // search with feature uniquename
    $feature_ids = explode(',', $heatmap_feature_uniquename);
    foreach($feature_ids as $feature_id){
      // trim white space at the beginning and the end
      $feature_id = trim($feature_id);
      $expressions[$feature_id] = db_query($sql_feature_uniquename, array(":feature_uniquename" => $feature_id))->fetchAllAssoc('elementresult_id');
    }
  } 

  if(!empty($expressions)){
    // Get all biomaterial names from different organisms
    $biomaterials = array();
    foreach($expressions as $features){
      // each feature may have may have multiple expression data from different treatment
      foreach($features as $feature){
        $biomaterials[$feature->name] = $feature->name;
      }
    }
  
    // Loop through all biomaterial so that each feature have all biomaterials (treatment)
    // Assign NULL as expression intensity to all treatment.
    $expression_data = array();
    foreach($expressions as $feature_id=>$features){
      foreach($biomaterials as $biomaterial_id){
            $expression_data[$feature_id][$biomaterial_id] = NULL;
      }   
    }

    // Foreach feature, loop through the biomaterials it has and get the signal value (expression data)
    foreach($expressions as $feature_id=>$features){
      foreach($features as $feature){

            // get analysis URL
            $analysis_url = '';
            $analysis_id = $feature->analysis_id;
            $sql_analysis = "SELECT * FROM chado.analysis WHERE analysis_id=:analysis_id";
            if(!empty($analysis_id)){
              $analysis = db_query($sql_analysis, array(':analysis_id'=>$analysis_id))->fetchAllAssoc('analysis_id');        
              // analysis page url consists of analysis name, sourcename, program and programversion
              $analysis_url = $analysis[$analysis_id]->name.', '.
                              $analysis[$analysis_id]->sourcename.' ('.
                              $analysis[$analysis_id]->program.', '.
                              $analysis[$analysis_id]->programversion.')';
              $analysis_url = preg_replace("/[^A-Za-z0-9 ]/", '', $analysis_url); // remove all non-alphanumeric charaters
              $analysis_url = preg_replace("/\s/", '-', $analysis_url); // replace ' ' with '-'
              $analysis_url = 'content/'.strtolower($analysis_url);
            }
            // get treatment
            $treat = chado_get_property(
              array('table' => 'biomaterial', 'id' => $feature->biomaterial_id),
              array('type_name' => 'treatment', 'cv_name' => 'biomaterial_property')
            );

            if($treat) { 
              $treatment = $treat->value;
              // link analysis_url to treatment
              $treatment = l($treatment, $analysis_url);
            }
            else {
              $treatment = '';
            }

            $expression_data[$feature_id][$feature->name]= array('signal'=>(float)$feature->signal, 
                                                                 'description'=>$feature->description,
                                                                 'quantification_name'=>$treatment,
                                                                 'analysis_url'=>$analysis_url);
      }
    }
  
    $x = array();
    $y = array();
    $z = array();
    $text = array();
    $max_id_len = 0;
    foreach($expression_data as $feature_id=>$values){
      // obtain max feature id length
      if(strlen($feature_id)>$max_id_len){
        $max_id_len = strlen($feature_id);
      }
      $y[] = '<a href="/feature/'.$feature_id.'">'.$feature_id.'</a>';
      $x = array_keys($values);
      $signals = array();
      $descriptions = array();
      foreach($values as $value){
        $signals[] = $value['signal'];
        $descriptions[] = 'Treatment: '.$value['quantification_name'].'<br>'.'Description: '.$value['description'];
      }
      $z[] = $signals;
      $text[] = $descriptions; 
    }

    // obtain max length of treatment names
    $max_treatment_name = 0;
    foreach($x as $name){
      if(strlen($name)>$max_treatment_name){
        $max_treatment_name = strlen($name);
      }
    }

    // use the max id length times 8 to get an appropriate pixel number for left margin
    $_SESSION['left_margin'] = $max_id_len * 8;
    // use the max length of treatment name times 7 to get an appropriate pixel number of bottom margin
    $_SESSION['bottom_margin'] = $max_treatment_name * 9;

    // Build variable for plotly function in js file.
    // Note that this variable requires that the x, y, z and type are not within double quotations
    // To build this variable, we first create a string, and then use javascripts' eval() function
    // to convert this string into a javascript variable.
    // The eval() function is called in the heatmap.js file. 
    if(!empty($x) and !empty($y) and !empty($x)){
      $_SESSION['heatmap_data'] = '[{'.'
                                      x:'.json_encode($x).', 
                                      y:'.json_encode($y).', 
                                      z:'.json_encode($z).', 
                                      text:'.json_encode($text).',
                                      hoverinfo: "all",
                                      type:"heatmap"}]';
    } else {
      $_SESSION['heatmap_data'] = '';
    }
  }
  // add SESSION downloadLink
  $feature_ids_string=NULL;
  foreach($expressions as $features){
    foreach($features as $feature){
      $id = $feature->feature_id;
      if (is_null($feature_ids_string)){
         $feature_ids_string = $id;
      }
      elseif (strpos($feature_ids_string,$id) === FALSE){
        $feature_ids_string .=  ',' . $id ;
      }
    }
  }
  $_SESSION['downloadLink'] =  url('chado/expression/csv', array('query' => array('feature_id'=>$feature_ids_string)));



  /*
  $_SESSION['heatmap_x'] = $x;
  $_SESSION['heatmap_y'] = $y;
  $_SESSION['heatmap_z'] = $z;
  */
}  
