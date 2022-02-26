<?php 

/**
 * 
 */
function tripal_analysis_expression_feature_viewer_form($form, &$form_state, 
    $feature_id = NULL) {  
      
  global $user;
  
  $hide_expression = variable_get('chado_feature_expression_hide_expression');
  $hide_biomaterial_labels = variable_get('chado_feature_expression_hide_biomaterial_labels');
  $limit_label_length = trim(variable_get('chado_feature_expression_limit_label_length'));
  $expression_display = variable_get('chado_feature_expression_expression_display');
  $biomaterial_display_width = variable_get('chado_feature_expression_biomaterial_display_width');
  $min_num_biomaterials = variable_get('chado_feature_expression_min_num_biomaterials');
  
      
  // Set default form values.
  $analysis_id = 0;
  if (array_key_exists('values', $form_state)) {
    $feature_id = array_key_exists('feature_id', $form_state['values']) ? $form_state['values']['feature_id'] : $feature_id;
    $analysis_id = array_key_exists('analysis_id', $form_state['values']) ? $form_state['values']['analysis_id'] : 0;
  }  
    
  // Default settings to return.
  $settings = [
    'analysis_id' => $analysis_id,
    'feature_id' => $feature_id,
    'allprops' => json_encode([]),
    'heatMapRaw' => json_encode([
      'biomaterials' => [
        [
          'intensity' => [],
          'properties' => []          
        ]
      ],
    ]),
    'maxLength' => $limit_label_length,
    'showLabels' => $hide_biomaterial_labels,
    'col' => $expression_display,
    'colWidth' => $biomaterial_display_width,
    'allprops' => json_encode([]),
    'num_samples' => 0,
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
    
    // Add these data values to the JSON so that the viewer can work.
    $data = tripal_analysis_expression_feature_viewer_get_data($feature_id,
        $analysis_id, $analyses[$analysis_id]);
    $settings = [
      'heatMapRaw' => $data['json_exp'],
      'maxLength' => $limit_label_length,
      'showLabels' => $hide_biomaterial_labels,
      'col' => $expression_display,
      'colWidth' => $biomaterial_display_width,      
      'analysis_id' => $analysis_id,
      'feature_id' => $feature_id,      
      'allprops' => json_encode($data['allprops']),
      'num_samples' => $data['num_samples']
    ];
    
    $biosample_props = $data['allprops'];
    $form['property_sort'] = [
      '#type' => 'select',
      '#title' => 'Order By',
      '#attributes' => ['class' => ['expression-viewer-order-by']],
      '#description' => 'Select a property to group and sort samples',
      '#options' => $biosample_props,
    ];
    $form['property_color'] = [
      '#type' => 'select',
      '#title' => 'Color By',
      '#attributes' => ['class' => ['expression-viewer-color-by']],
      '#description' => 'Select a property to color samples',
      '#options' => $biosample_props,
    ];
    $form['shown_non_zero'] = [
      '#type' => 'checkbox',
      '#title' => 'Exclude zero values',
      '#attributes' => ['class' => ['expression-viewer-exclude-zero']],
    ];
        
    $form['instructions'] = [
      '#type' => 'markup',
      '#markup' => '<p>Hover the mouse over a column in the graph to view more
        information about that biological sample. You can click and drag to
        rearrange groups along the x-axis. You can also click and drag to move
        the legend.</p>'
    ];  
    
    $form['download'] = [
      '#type' => 'markup',
      '#markup' => '
        <a href="/tripal/analysis-expression/download?feature_ids=' . $feature_id . '&analysis_id=' . $analysis_id .'" id="expressionDownloadLink">
         Download expression dataset for this feature
      </a>'
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
   
  $has_exp = FALSE;
  $unit_cvterm = chado_get_cvterm(['id' => 'sep:00056']);
  
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
  $args =[
    ":feature_id" => $feature_id, 
    ":analysis_id" => $analysis_id,
  ];
  $results = chado_query($sql, $args);
    
  $signals = [];
  $biomaterials = [];
  $units = [];
  $properties = [];
  $allprops = [];
  $allprops["Biosample Name"] = 1;
  
  while($exp = $results->fetchObject()) {
    $has_exp = TRUE;
   
    $signals[$exp->biomaterial_id] = $exp->signal;
    $biomaterials[$exp->biomaterial_id] = [
      'name' => $exp->name,
      'description' => $exp->description,
    ];
    $properties[$exp->biomaterial_id] = [];    
  }
  
  // Get the biosample properties.
  $sql = "
    SELECT BP.biomaterial_id, CVT.name, BP.value
    FROM {biomaterialprop} BP
      INNER JOIN {cvterm} CVT on CVT.cvterm_id = BP.type_id
    WHERE BP.biomaterial_id IN (:biomaterial_ids)
    ORDER by biomaterial_id, name
  ";
  $args = [':biomaterial_ids' => array_keys($biomaterials)];
  $results = chado_query($sql, $args);
  while ($property = $results->fetchObject()) {
    $properties[$property->biomaterial_id]["Biosample Name"] = $biomaterials[$property->biomaterial_id]['name'];    
    if ($property->value) {
      $prop_name = ucfirst($property->name);
      $properties[$property->biomaterial_id][$prop_name] = $property->value;
      $allprops[$prop_name] = 1;    
    }
  }
  $allprops = array_keys($allprops);   
  
  // create the array expected by the JS script
  $data = [];
  foreach ($biomaterials as $biomaterial_id => $info) {
    $node_present = chado_get_record_entity_by_table('biomaterial', $biomaterial_id);
    
    $data['biomaterials'][] = [
      "name" => $info['name'],
      "intensity" => $signals[$biomaterial_id],
      "units" => $units[$biomaterial_id],
      "description" => $info['description'],
      "properties" => $properties[$biomaterial_id],
      "node" => $node_present,
    ];
  }
  
  return [    
    'allprops' => $allprops,
    'has_exp' => $has_exp,
    'num_samples' => count(array_keys($biomaterials)),    
    'json_exp' => json_encode($data),
  ];
}