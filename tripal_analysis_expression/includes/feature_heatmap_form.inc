<?php

/**
 * Escape HTML special characters in a string.
 *
 * @param  $value
 *
 * @return string
 */
function e($value) {
  return htmlspecialchars($value, ENT_QUOTES, 'UTF-8', FALSE);
}

/**
 * Heatmap settings form.
 *
 * @implements HOOK_form()
 *
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function tripal_analysis_expression_heatmap_settings_form($form, &$form_state) {
  $form['help_section'] = [
    '#type' => 'markup',
    '#markup' => '<h3>Expression Visualisation Settings</h3>',
  ];

  $form['placeholder'] = [
    '#type' => 'textarea',
    '#title' => 'Placeholder Text',
    '#description' => 'The placeholder text for the text box.',
    '#required' => TRUE,
    '#default_value' => variable_get('tripal_analysis_expression_heatmap_placeholder'),
  ];

  $form['example'] = [
    '#type' => 'textarea',
    '#title' => 'Example Value',
    '#description' => 'The example generated when the user clicks the "try an example" button',
    '#required' => TRUE,
    '#default_value' => variable_get('tripal_analysis_expression_heatmap_example'),
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Save',
  ];

  return $form;
}

/**
 * Heatmap settings form submit.
 *
 * @implements HOOK_form_submit()
 *
 * @param $form
 * @param $form_state
 */
function tripal_analysis_expression_heatmap_settings_form_submit($form, &$form_state) {
  $placeholder = $form_state['values']['placeholder'];
  $example = $form_state['values']['example'];

  variable_set('tripal_analysis_expression_heatmap_placeholder', $placeholder);
  variable_set('tripal_analysis_expression_heatmap_example', $example);

  drupal_set_message('Settings saved successfully');
}

/**
 * Create the heatmap form
 *
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function feature_heatmap_form($form, &$form_state) {
  $form['descriptions'] = [
    '#type' => 'item',
    '#markup' => t('<b>Feature unique names need to be separated by comma, spaces, tabs or newlines.</b>'),
  ];

  $form['example_button'] = [
    '#type' => 'button',
    '#value' => 'Try an Example',
    '#attributes' => [
      'class' => ['pull-right', 'btn-info'],
    ],
  ];

  $example = variable_get('tripal_analysis_expression_heatmap_example');
  $form['heatmap_feature_uniquename'] = [
    '#type' => 'textarea',
    '#title' => t('Enter feature unique names'),
    '#description' => t('<b>Example feature unique names:</b> ' . $example),
    '#attributes' => [
      'placeholder' => variable_get('tripal_analysis_expression_heatmap_placeholder'),
      'id' => 'heatmap_feature_uniquename',
    ],
  ];

  // Escaped get value if it exists
  if (isset($_GET['heatmap_feature_uniquename']) && !empty($_GET['heatmap_feature_uniquename'])) {
    $form['heatmap_feature_uniquename']['#value'] = e($_GET['heatmap_feature_uniquename']);
  }

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => t('Display Expression Heatmap'),
  ];

  $form['#method'] = 'get';

  return $form;
}

/*
 * feature_heatmap_form submit handler
 */
function feature_heatmap_form_submit($form, &$form_state) {

}

function tripal_analysis_expression_generate_heatmap_values() {
  // Escaped get value if it exists
  if (!isset($_GET['heatmap_feature_uniquename']) || empty($_GET['heatmap_feature_uniquename'])) {
    return NULL;
  }

  $heatmap_feature_uniquename = $_GET['heatmap_feature_uniquename'];

  $sql = "SELECT ER.elementresult_id, ER.signal, F.feature_id, F.uniquename, 
            Q.name AS quantification_name, Q.analysis_id, 
            B.name, B.biomaterial_id, B.description FROM {elementresult} ER
            INNER JOIN {element} E ON E.element_id = ER.element_id
            INNER JOIN {feature} F ON F.feature_id = E.feature_id
            INNER JOIN {quantification} Q ON Q.quantification_id = ER.quantification_id
            INNER JOIN {acquisition} AQ ON AQ.acquisition_id = Q.acquisition_id
            INNER JOIN {assay} A ON A.assay_id = AQ.assay_id
            INNER JOIN {assay_biomaterial} AB ON AB.assay_id = A.assay_id
            INNER JOIN {biomaterial} B ON B.biomaterial_id = AB.biomaterial_id
            WHERE F.uniquename IN (:feature_uniquename)";

  $expressions = [];

  if (!empty($heatmap_feature_uniquename)) {  // search with feature uniquename
    $feature_ids = explode(',', $heatmap_feature_uniquename);

    if (count($feature_ids) < 2) {
      return FALSE;
    }

    $expressions = [];
    // Trim white space at the beginning and the end
    foreach ($feature_ids as $key => $feature_id) {
      $feature_ids[$key] = $feature_id = trim($feature_id);
      $expressions[$feature_id] = [];
    }

    $expressions_all = chado_query($sql, [":feature_uniquename" => $feature_ids])->fetchAll();

    foreach ($expressions_all as $expression) {
      $expressions[$expression->uniquename][] = $expression;
    }
  }

  if (empty($expressions)) {
    return NULL;
  }

  // Get all biomaterial names from different organisms
  $biomaterials = [];
  foreach ($expressions as $features) {
    // each feature may have may have multiple expression data from different treatment
    foreach ($features as $feature) {
      $biomaterials[$feature->name] = $feature->name;
    }
  }

  // Loop through all biomaterial so that each feature have all biomaterials (treatment)
  // Assign NULL as expression intensity to all treatment.
  $expression_data = [];
  $ids = [];
  foreach ($expressions as $feature_id => $features) {
    foreach ($biomaterials as $biomaterial_id) {
      $expression_data[$feature_id][$biomaterial_id] = NULL;
    }
  }

  // Foreach feature, loop through the biomaterials it has and get the signal value (expression data)
  foreach ($expressions as $feature_id => $features) {
    foreach ($features as $feature) {
      $ids[$feature_id] = $feature->feature_id;

      // get analysis URL
      $analysis_url = '';
      $analysis_id = $feature->analysis_id;
      if (!empty($analysis_id)) {
        $analysis_url = 'bio_data/' . tripal_get_chado_entity_id('analysis', $analysis_id);
      }

      // get treatment
      $treat = chado_get_property([
        'table' => 'biomaterial',
        'id' => $feature->biomaterial_id,
      ], ['type_name' => 'treatment', 'cv_name' => 'biomaterial_property']);

      if ($treat) {
        $treatment = $treat->value;
        // link analysis_url to treatment
        $treatment = l($treatment, $analysis_url);
      }
      else {
        $treatment = '';
      }

      $expression_data[$feature_id][$feature->name] = [
        'signal' => (float) $feature->signal,
        'description' => $feature->description,
        'quantification_name' => $treatment,
        'analysis_url' => $analysis_url,
      ];
    }
  }

  $x = [];
  $y = [];
  $z = [];
  $text = [];
  $max_id_len = 0;
  foreach ($expression_data as $feature_id => $values) {
    // obtain max feature id length
    $max_id_len = max(strlen($feature_id), $max_id_len);

    if (isset($ids[$feature_id])) {
      $y[] = l($feature_id, 'bio_data/' . tripal_get_chado_entity_id('feature', $ids[$feature_id]));
    }
    else {
      $y[] = $feature_id;
    }

    $x = array_keys($values);
    $signals = [];
    $descriptions = [];
    foreach ($values as $value) {
      $signals[] = $value['signal'];
      $descriptions[] = 'Treatment: ' . $value['quantification_name'] . '<br>' . 'Description: ' . $value['description'];
    }
    $z[] = $signals;
    $text[] = $descriptions;
  }

  // obtain max length of treatment names
  $max_treatment_name = 0;
  foreach ($x as $name) {
    if (strlen($name) > $max_treatment_name) {
      $max_treatment_name = strlen($name);
    }
  }

  // use the max id length times 8 to get an appropriate pixel number for left margin
  $left_margin = ($max_id_len * 12) / 1.5;
  // use the max length of treatment name times 7 to get an appropriate pixel number of bottom margin
  $bottom_margin = ($max_treatment_name * 12) / 1.5;

  // Build data for plotly function in js file.
  $heatmap_data = [];

  if (!empty($x) and !empty($y) and !empty($z)) {
    $heatmap_data = [
      [
        'x' => $x,
        'y' => $y,
        'z' => $z,
        'text' => $text,
        'hoverinfo' => 'all',
        'type' => 'heatmap',
      ],
    ];
  }

  return [
    'heatmap_data' => $heatmap_data,
    'bottom_margin' => $bottom_margin,
    'left_margin' => $left_margin,
  ];
}

function feature_heatmap_search_form($form, &$form_state) {
  $form['search_term'] = [
    '#type' => 'textfield',
    '#title' => t('Search Features'),
    '#description' => 'Search by name or accession.',
    '#attributes' => [
      'placeholder' => t('Example: IPR003576'),
      'id' => 'heatmap-search_term',
      'autocomplete' => 'off',
      'maxlength' => '',
      'size' => '',
    ],
  ];

  $form['search_results_fieldset'] = [
    '#markup' => '<div id="feature-heatmap-search-results-wrapper"><div id="heatmap-form-throbber"></div><div id="feature-heatmap-search-results"></div></div>',
    '#type' => 'markup',
  ];

  return $form;
}

/**
 * Search features that have expression data.
 */
function tripal_analysis_expression_search_features_callback() {
  if (!isset($_GET['terms']) || empty($_GET['terms'])) {
    drupal_json_output(['data' => []]);
    return;
  }

  $sql = "SELECT EF.feature_uniquename AS uniquename,
                 EF.organism_common_name AS common_name,
                 EF.accession AS accession 
            FROM {expression_feature} EF
            WHERE LOWER(EF.feature_uniquename) LIKE :feature_uniquename 
                  OR LOWER(EF.accession) LIKE :accession";

  if (preg_match('/^IPR|^GO/i', $_GET['terms']) !== 0) {
    $sql .= " AND EF.accession IS NOT NULL";
  }

  $sql .= " ORDER BY feature_uniquename
            LIMIT 20";

  $term = '%' . db_like(strtolower($_GET['terms'])) . '%';

  $results = chado_query($sql, [
    ':feature_uniquename' => $term,
    //':organism_name' => $term,
    ':accession' => $term,
  ]);

  if (!$results) {
    drupal_json_output(['data' => []]);
    return;
  }

  drupal_json_output(['data' => $results->fetchAll()]);
  return;
}
