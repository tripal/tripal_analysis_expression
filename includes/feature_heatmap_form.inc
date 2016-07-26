<?php

function feature_heatmap_form($form, &$form_state){
  $form['descriptions'] = array(
    '#type' => 'item',
    '#markup' => t('<b>If the feature unique names is not empty, it will search by feature unique names.
                   otherwise, it will search by feature IDs.</b>')
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

  $sql_feature_uniquename = "SELECT ER.elementresult_id, ER.signal, F.feature_id, F.uniquename, Q.name AS quantification_name, B.name, B.biomaterial_id, B.description FROM {chado.elementresult} ER
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
            $expression_data[$feature_id][$feature->name]= array('signal'=>(float)$feature->signal, 
                                                                 'description'=>$feature->description,
                                                                 'quantification_name'=>$feature->quantification_name);
      }
    }
  
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

    // use the max id length times 8 to get an appropriate pixel number for left margin
    $_SESSION['left_margin'] = $max_id_len * 8;

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

  /*
  $_SESSION['heatmap_x'] = $x;
  $_SESSION['heatmap_y'] = $y;
  $_SESSION['heatmap_z'] = $z;
  */
}  
