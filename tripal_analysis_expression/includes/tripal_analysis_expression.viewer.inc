<?php 

/**
 * 
 */
function tripal_analysis_expression_feature_viewer_form($form, &$form_state, 
    $feature_id = NULL) {  
      
  global $user;
      
  // Set default form values.
  $analysis_id = 0;
  if (array_key_exists('values', $form_state)) {
    $feature_id = array_key_exists('feature_id', $form_state['values']) ? $form_state['values']['feature_id'] : $feature_id;
    $analysis_id = array_key_exists('analysis_id', $form_state['values']) ? $form_state['values']['analysis_id'] : 0;
  }  
    
  // Default settings to return.
  $settings = [
    'heatMapRaw' => json_encode(['biomaterials' => []]),
    'maxLength' => 0,
    'showLabels' => 0,
    'col' => 0,
    'colWidth' => 0,
    'selectedAnalysis' => $analysis_id,
    'feature_id' => $feature_id
  ];
  
  // Start the form wrapper for AJAX.
  $form['viewer_wrapper_start'] = [
    '#type' => 'markup',
    '#markup' => '<div id="tripal-analysis-expression-feature-viewer-form">'
  ];
  
  // Get the list of analyses:
  $sql = "
    SELECT AN.analysis_id, AN.name
    FROM {elementresult} ER
      INNER JOIN {element} E ON E.element_id = ER.element_id
      INNER JOIN {feature} F ON F.feature_id = E.feature_id
      INNER JOIN {quantification} Q ON Q.quantification_id = ER.quantification_id
      INNER JOIN {acquisition} AQ ON AQ.acquisition_id = Q.acquisition_id
      INNER JOIN {assay} A ON A.assay_id = AQ.assay_id
      INNER JOIN {assay_biomaterial} AB ON AB.assay_id = A.assay_id
      INNER JOIN {biomaterial} B ON B.biomaterial_id = AB.biomaterial_id
      INNER JOIN {analysis} AN ON AN.analysis_id = Q.analysis_id
    WHERE F.feature_id = :feature_id
    ORDER BY AN.name
  ";
  $results = chado_query($sql, [":feature_id" => $feature_id]);
  $analyses = [0 => '--Select--'];
  while ($analysis = $results->fetchObject()) {
    // Only show analyses the user has permission tovie.
    $entity_id = chado_get_record_entity_by_table('analysis', $analysis->analysis_id);
    if ($entity_id){
      $entity = tripal_load_entity('TripalEntity', [$entity_id]);
      if (tripal_entity_access('view', $entity[$entity_id], $user, 'TripalEntity')) {
        $analyses[$analysis->analysis_id] = $analysis->name;
      }
    }
  }
  
  $form['feature_id'] = [
    '#type' => 'value',
    '#value' => $feature_id,
  ];
  
  //$analyses[$key]->url = 'bio_data/' . chado_get_record_entity_by_table('analysis', $analysis->analysis_id);
  $form['analysis_id'] = [
    '#type' => 'select',
    '#title' => 'Analysis',
    '#options' => $analyses,
    '#description' => 'Select an expression analysis for viewing',    
    '#attributes' => ['class' => ['analyses-dropdown']],
    '#default_value' => $analysis_id,
    '#ajax' => [
      'callback' => 'tripal_analysis_expression_feature_viewer_form_ajax_callback',
      'wrapper'  => 'tripal-analysis-expression-feature-viewer-form',
      'effect'   => 'fade',
      'method'   => 'replace',
    ],
  ];
  
  if ($analysis_id) {
  
    $form['property-sort-div'] = [
      '#type' => 'markup',
      '#markup' => '<p id="propertySortDiv"><strong>Select a property to group and sort biological samples</strong></p>',
    ];
    
    $form['property-color-div'] = [
      '#type' => 'markup',
      '#markup' => '<p id="propertyColorDiv"><strong>Select a property to color biological samples</strong></p>',
    ];
    
    $form['instructions'] = [
      '#type' => 'markup',
      '#markup' => '<p>Hover the mouse over a column in the graph to view more 
        information about that biological sample. You can click and drag to 
        rearrange groups along the x-axis. You can also click and drag to move 
        the legend.</p>'
    ];
    
    $form['checks'] = [
      '#type' => 'markup',
      '#markup' => '
        <p>
          <a href="javascript:;" id="show-non-zero-only">Only Non-Zero Values</a> |
          <a href="javascript:;" id="reset-expression-plot">Reset</a>
        </p>',
    ];    
    
    $form['download'] = [
      '#type' => 'markup',
      '#markup' => '
        <a href="/tripal/analysis-expression/download?feature_ids=' . $feature_id . '&analysis_id=' . $analysis_id .'" id="expressionDownloadLink">
         Download expression dataset for this feature
      </a>'
    ];

    // Add these data values to the JSON so that the viewer can work.
    $data = tripal_analysis_expression_feature_viewer_get_data($feature_id, 
        $analysis_id, $analyses[$analysis_id]);
    $settings = [
      'heatMapRaw' => $data['json_exp'],
      'maxLength' => $data['limit_label_length'],
      'showLabels' => $data['hide_biomaterial_labels'],
      'col' => $data['expression_display'],
      'colWidth' => $data['biomaterial_display_width'],
      'selectedAnalysis' => $analysis_id,
      'feature_id' => $feature_id      
    ];
  }
  
  $form['viewer_wrapper_end'] = [
    '#type' => 'markup',
    '#markup' => '</div>'
  ];
  
  drupal_add_js($settings, 'setting');
  return $form;
}

/**
 * 
 */
function tripal_analysis_expression_feature_viewer_form_ajax_callback($form, $form_state){
  return $form;
}
/**
 * 
 */
function tripal_analysis_expression_feature_viewer_get_data($feature_id, $analysis_id, $analysis_name) {
   
  $hide_expression = variable_get('chado_feature_expression_hide_expression');
  $hide_biomaterial_labels = variable_get('chado_feature_expression_hide_biomaterial_labels');
  $limit_label_length = trim(variable_get('chado_feature_expression_limit_label_length'));
  $expression_display = variable_get('chado_feature_expression_expression_display');
  $biomaterial_display_width = variable_get('chado_feature_expression_biomaterial_display_width');
  $min_num_biomaterials = variable_get('chado_feature_expression_min_num_biomaterials');
  
  $has_exp = FALSE;
  
  $sql = "
    SELECT 
      ER.signal, B.name, B.biomaterial_id, B.description, 
      Q.quantification_id, AN.analysis_id, AN.name AS analysis_name
    FROM {elementresult} ER
      INNER JOIN {element} E ON E.element_id = ER.element_id
      INNER JOIN {feature} F ON F.feature_id = E.feature_id
      INNER JOIN {quantification} Q ON Q.quantification_id = ER.quantification_id
      INNER JOIN {acquisition} AQ ON AQ.acquisition_id = Q.acquisition_id
      INNER JOIN {assay} A ON A.assay_id = AQ.assay_id
      INNER JOIN {assay_biomaterial} AB ON AB.assay_id = A.assay_id
      INNER JOIN {biomaterial} B ON B.biomaterial_id = AB.biomaterial_id
      INNER JOIN {analysis} AN ON AN.analysis_id = Q.analysis_id
    WHERE F.feature_id = :feature_id and AN.analysis_id = :analysis_id
      ORDER BY AN.analysis_id 
  ";
  $results = chado_query($sql, [":feature_id" => $feature_id, ":analysis_id" => $analysis_id]);
  $array_to_encode = [
    "analysis_id" => $analysis_id,
    "analysis_name" => $analysis_name,
    "biomaterials" => [],
  ];
    
  while($exp = $results->fetchObject()) {
    $has_exp = TRUE;
    
    //Get units in quantificationprop
    $unit_prop = chado_get_property([
      'table' => 'quantification',
      'id' => $exp->quantification_id,
    ], ['type_name' => 'unit_of_measure', 'cv_name' => 'sep']);
    $unit_prop ? $units = $unit_prop->value : $units = '';
    
    //Get all properties for biosample/biomaterial
    $values = [
      'biomaterial_id' => $exp->biomaterial_id,
    ];
    $biomat_object = chado_generate_var('biomaterial', $values);
    $result = chado_expand_var($biomat_object, 'table', 'biomaterialprop');
    $properties = $result->biomaterialprop;
    $propstore = [];
    
    if (is_object($properties)) {
      $properties = [];
    }
    
    if (!$properties) {
      $properties = [];
    }
    
    foreach ($properties as $prop) {
      //get the prop name and value
      // $property_cvterm_id = $prop->type_id->cvterm_id;
      // $property_cv_id = $prop->type_id->cv_id;
      if ((isset($prop->value))) { //for some reason we have stuff coming back that is not properties.  this will filter them.
        $property_name = $prop->type_id->name;
        $property_value = $prop->value;
        $propstore[$property_name] = $property_value;
      }
    }
    
    $node_present = chado_get_record_entity_by_table('biomaterial',
        $exp->biomaterial_id);
    
    //Add a property for no sorting ie by biosample name
    $propstore["Biosample Name"] = $exp->name;
    
    //Build entry for this biosample/biomaterial, append it to the existing array, and store it in the master array
    $biosample_properties = [
      "name" => $exp->name,
      "intensity" => $exp->signal,
      "units" => $units,
      "description" => $exp->description,
      "properties" => $propstore,
      "node" => $node_present,
    ];
    $existing_samples = $array_to_encode["biomaterials"];
    array_push($existing_samples, $biosample_properties);
    $array_to_encode["biomaterials"] = $existing_samples;
  }
  
  $json_exp = json_encode($array_to_encode);
  
  if ($hide_expression == 1) {
    $json_exp = "";
  }
  

  
  $data = [    
    'has_exp' => $has_exp,
    'json_exp' => $json_exp,
    'limit_label_length' => $limit_label_length,
    'hide_expression' => $hide_expression,
    'hide_biomaterial_labels' => $hide_biomaterial_labels,
    'expression_display' => $expression_display,
    'biomaterial_display_width' => $biomaterial_display_width,
    'min_num_biomaterials' => $min_num_biomaterials,
  ];
  
  return $data;
}