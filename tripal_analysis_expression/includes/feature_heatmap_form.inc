<?php

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
    '#markup' => '<h3>Expression Heatmap Search Settings</h3>',
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

  $form['use_elasticsearch'] = [
    '#type' => 'checkbox',
    '#title' => 'Enable elasticsearch integration. (Requires installing tripal_elasticsearch module)',
    '#default_value' => variable_get('analysis_expression_heatmap_db') === 'elasticsearch',
  ];

  $form['submit'] = [
    '#type' => 'submit',
    '#value' => 'Save',
  ];

  return $form;
}

function tripal_analysis_expression_heatmap_settings_form_validate(
  $form,
  &$form_state
) {
  $use_elasticsearch = $form_state['values']['use_elasticsearch'];

  if ($use_elasticsearch && !module_exists('tripal_elasticsearch')) {
    form_set_error('use_elasticsearch', 'Tripal Elasticsearch module is not installed. Please install and enable it first before attempting to enable Elasticsearch integration.');
  }
}

/**
 * Heatmap settings form submit.
 *
 * @implements HOOK_form_submit()
 *
 * @param $form
 * @param $form_state
 */
function tripal_analysis_expression_heatmap_settings_form_submit(
  $form,
  &$form_state
) {
  $placeholder = $form_state['values']['placeholder'];
  $example = $form_state['values']['example'];
  $use_elasticsearch = $form_state['values']['use_elasticsearch'];

  variable_set('tripal_analysis_expression_heatmap_placeholder', $placeholder);
  variable_set('tripal_analysis_expression_heatmap_example', $example);
  variable_set('analysis_expression_heatmap_db', $use_elasticsearch ? 'elasticsearch' : 'mview');

  drupal_set_message('Settings saved successfully');

  if ($use_elasticsearch) {
    try {
      $es = new ESInstance();
      $indices = $es->getIndices();

      // Create the index if does't exist.
      if (!in_array('expression_feature', $indices)) {
        $es->setIndexParams('expression_feature', 5, 0, 'standard', [], [
          'feature_id' => 'integer',
          'organism_id' => 'integer',
          'feature_uniquename' => 'string',
          'organism_common_name' => 'string',
          'accession' => 'string',
        ])->createIndex();
      }

      global $user;
      $job_name = 'Populate Elasticsearch expression feature index';
      $job_module = 'tripal_analysis_expression';
      $job_callback = 'tripal_analysis_expression_populate_es_index';
      tripal_add_job($job_name, $job_module, $job_callback, [], $user->uid);
    } catch (Exception $exception) {
      drupal_set_message($exception->getMessage(), 'error');
      return;
    }
    drupal_set_message('Elasticsearch indexing job initiated. Make sure cron is running.');
  }
}

/**
 * Populate the expression_feature index.
 *
 * @throws \Exception
 */
function tripal_analysis_expression_populate_es_index() {
  try {
    $es = new ESInstance();
  } catch (Exception $exception) {
    print $exception->getMessage();
    return;
  }

  tripal_analysis_expression_insert_es_index();

  $sql = 'SELECT COUNT(feature_id) FROM {expression_feature}';
  $result = chado_query($sql);
  if (!$result) {
    throw new Exception('Couldn\'t get data from materialized view. Please make sure that the expression_feature materialized view is populated.');
  }
  $count = $result->fetchObject();
  $count = intval($count->count);

  $index = 'expression_feature';
  print "Clearing index of all records.\n";
  $es->deleteAllRecords($index);

  print "Populating index please wait.\n";

  for ($i = 0; $i < $count; $i + 500 < $count ? ($i += 500) : ($i += $count - $i - 1)) {
    $sql = 'SELECT feature_id, organism_id, feature_uniquename, organism_common_name, accession
            FROM {expression_feature} OFFSET :offset LIMIT 500';
    $result = chado_query($sql, [
      ':offset' => $i,
    ]);
    $records = $result->fetchAll();

    $es->bulkIndex($index, $records);

    print number_format(($i / $count) * 100) . "%\r";
  }

  print "\n\nIndex populated successfully.\n";
}

/**
 * If the index entry doesn't exist then create it
 *    in `public.tripal_elasticsearch_indices`
 */
function tripal_analysis_expression_insert_es_index() {
  $query = "SELECT COUNT(*) AS count FROM {tripal_elasticsearch_indices} WHERE index_name='expression_feature'";
  $result = db_query($query);
  if ($result->fetchField() === 0) {
    //insert index record into database for all types.
    $insert = [
      ':index_name' => 'expression_feature',
      ':table_name' => 'expression_feature',
      ':exposed' => 0,
    ];
    db_query('INSERT INTO {tripal_elasticsearch_indices} (index_name, TABLE_NAME, exposed) VALUES (:index_name, :table_name, :exposed)', $insert);
  }
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
    $form['heatmap_feature_uniquename']['#value'] = $_GET['heatmap_feature_uniquename'];
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

/**
 * Generate heat map values.
 *
 * @return array|boolean
 */
function tripal_analysis_expression_generate_heatmap_values() {
  // Escaped get value if it exists
  if (!isset($_GET['heatmap_feature_uniquename']) || empty($_GET['heatmap_feature_uniquename'])) {
    return NULL;
  }

  $heatmap_feature_uniquename = $_GET['heatmap_feature_uniquename'];

  $sql = "SELECT ER.elementresult_id, ER.signal, F.feature_id, F.uniquename, 
            Q.name AS quantification_name, Q.analysis_id, Q.quantification_id,
            B.name, B.biomaterial_id, B.description, AN.analysis_id, AN.name AS analysis_name FROM {elementresult} ER
            INNER JOIN {element} E ON E.element_id = ER.element_id
            INNER JOIN {feature} F ON F.feature_id = E.feature_id
            INNER JOIN {quantification} Q ON Q.quantification_id = ER.quantification_id
            INNER JOIN {acquisition} AQ ON AQ.acquisition_id = Q.acquisition_id
            INNER JOIN {assay} A ON A.assay_id = AQ.assay_id
            INNER JOIN {assay_biomaterial} AB ON AB.assay_id = A.assay_id
            INNER JOIN {biomaterial} B ON B.biomaterial_id = AB.biomaterial_id
            INNER JOIN {analysis} AN ON AN.analysis_id = Q.analysis_id
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

  // Loop through all biomaterials so that each feature have all biomaterials (treatment)
  // Assign NULL as expression intensity to all treatment.
  //TODO:  if we organize by analysis this no longer works.
  // While we do this, lets get the biomaterialprops
  $expression_data = [];
  $ids = [];
  $biomaterial_propstore = [];
  $analysis_legend = [];
  foreach ($expressions as $feature_id => $features) {
    //foreach ($biomaterials as $biomaterial_name) {
    //  $expression_data[$feature_id][$biomaterial_name] = NULL;
    //}
    foreach ($features as $biomaterial_properties) {
      $biomaterial_id = $biomaterial_properties->biomaterial_id;
      $biomaterial_name = $biomaterial_properties->name;

      $biomat_object = chado_generate_var('biomaterial', ['biomaterial_id' => $biomaterial_id]);

      $result = chado_expand_var($biomat_object, 'table', 'biomaterialprop');

      $properties = $result->biomaterialprop;
      if (!$properties) {
        $properties = [];
      }
      foreach ($properties as $prop) {
        //get the prop name and value
        // $property_cvterm_id = $prop->type_id->cvterm_id;
        // $property_cv_id = $prop->type_id->cv_id;
        $property_name = $prop->type_id->name;
        $property_value = $prop->value;
        $biomaterial_propstore[$biomaterial_name][$property_name] = $property_value;
      }
    }
  }

  // remove unknown features
  $features_count = count($expressions);
  $expressions = array_filter($expressions, function ($v) {
    return !empty($v);
  });
  if (count($expressions) < $features_count) {
    drupal_set_message('Unknown features have been removed from the list.', 'warning');
  }
  if (count($expressions) === 0) {
    drupal_set_message('All features you provided are unknown features. Please try again with a different set of feature names.', 'warning');
    return;
  }
  // Foreach feature, loop through the biomaterials it has and get the signal value (expression data)
  foreach ($expressions as $feature_id => $features) {
    foreach ($features as $feature) {

      $analysis_name = $feature->analysis_name;
      $ids[$feature_id] = $feature->feature_id;

      // get analysis URL
      $analysis_url = '';
      $analysis_id = $feature->analysis_id;
      if (!empty($analysis_id)) {
        $analysis_url = 'bio_data/' . chado_get_record_entity_by_table('analysis', $analysis_id);
      }

      $analysis_legend[$analysis_name] = $analysis_id;

      //Get units in quantificationprop
      $uni = chado_get_property([
        'table' => 'quantification',
        'id' => $feature->quantification_id,
      ], ['type_name' => 'unit_of_measure', 'cv_name' => 'sep']);
      $uni ? $units = $uni->value : $units = '';

      $treat = NULL;

      if (isset($biomaterial_propstore[$feature->name]["treatment"])) {
        $treat = $biomaterial_propstore[$feature->name]["treatment"];//this allows for treatment to be in a CV instead of specifying biomaterialprop
      }

      if ($treat) {
        $treatment = $treat;
        // link analysis_url to treatment
        $treatment = l($treatment, $analysis_url);
      }
      else {
        $treatment = '';
      }

      $expression_data[$analysis_name][$feature_id][$feature->name] = [
        'signal' => (float) $feature->signal,
        'description' => $feature->description,
        'treatment' => $treatment,
        'analysis_url' => $analysis_url,
        'biomaterial_properties' => isset($biomaterial_propstore[$feature->name]) ? $biomaterial_propstore[$feature->name] : [],
      ];
    }
  }


  $heatmap_data_by_analysis = [];
  $feature_id_list = [];


  foreach ($expression_data as $analysis_name => $features) {

    $x = [];
    $y = [];
    $z = [];
    $text = [];
    $max_id_len = 0;


    foreach ($features as $feature_id => $values) {

      // obtain max feature id length
      $max_id_len = max(strlen($feature_id), $max_id_len);

      if (isset($ids[$feature_id])) {
        $feature_id_list[$analysis_name][] = $ids[$feature_id];
        $y[] = l($feature_id, 'bio_data/' . chado_get_record_entity_by_table('feature', $ids[$feature_id]));
      }
      else {
        $y[] = $feature_id;
      }

      $x = array_keys($values);
      $signals = [];
      $descriptions = [];
      foreach ($values as $value) {
        $signals[] = $value['signal'];

        $biomaterial_props = $value['biomaterial_properties'];

        $property_html = '';

        if (is_array($biomaterial_props)) {
          foreach ($biomaterial_props as $property_name => $property_value) {
            $property_html .= '<br>' . $property_name . ":  " . $property_value;
          }
        }

        $descriptions[] = 'Treatment: ' . $value['treatment'] . '<br>' . 'Description: ' . $value['description'] . '<br>' . $property_html;
      }
      $z[] = $signals;
      $text[] = $descriptions;
    }


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

    $heatmap_data_by_analysis[$analysis_name] = $heatmap_data;

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


  return [
    'heatmap_data' => $heatmap_data_by_analysis,
    'bottom_margin' => $bottom_margin,
    'left_margin' => $left_margin,
    'feature_list' => $feature_id_list,
    'analysis_legend' => $analysis_legend,
  ];
}

/**
 * Create a search form.
 *
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
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

  $db = variable_get('analysis_expression_heatmap_db');

  if ($db === 'elasticsearch') {
    tripal_analysis_expression_search_features_es();
  }
  else {
    tripal_analysis_expression_search_features_mview();
  }
}

function tripal_analysis_expression_search_features_es() {
  $terms = '*' . strtolower($_GET['terms']) . '*';

  // Connect to ES
  try {
    $es = new ESInstance();
  } catch (Exception $exception) {
    // Fallback to use the mview since ES failed to connect.
    tripal_analysis_expression_search_features_mview();
    return;
  }

  try {
    $index = 'expression_feature';
    $query = [
      'bool' => [
        'should' => [
          [
            'wildcard' => [
              'feature_uniquename' => $terms,
            ],
          ],
          [
            'wildcard' => [
              'accession' => $terms,
            ],
          ],
          [
            'wildcard' => [
              'organism_common_name' => $terms,
            ],
          ],
        ],
      ],
    ];

    $results = $es->setTableSearchParams($index, $index, $query, [0, 20])
      ->search();

    $data = array_map(function ($result) {
      return [
        'uniquename' => $result['feature_uniquename'],
        'common_name' => $result['organism_common_name'],
        'accession' => $result['accession'],
      ];
    }, $results);

    drupal_json_output([
      'data' => $data,
      'error' => FALSE,
      'source' => 'ES',
    ]);
  } catch (Exception $exception) {
    drupal_set_message($exception->getMessage(), 'error');

    drupal_json_output([
      'data' => [],
      'error' => json_decode($exception->getMessage()),
      'source' => 'ES',
    ]);
  }
}

/**
 * Search using expression_feature mview.
 */
function tripal_analysis_expression_search_features_mview() {
  $terms = $_GET['terms'];

  $results = tripal_analysis_expression_search_features('accession', $terms);

  if (!$results) {
    $results = tripal_analysis_expression_search_features('feature_uniquename', $terms);
  }

  if (!$results) {
    $results = tripal_analysis_expression_search_features('organism_common_name', $terms);
  }

  if (!$results) {
    drupal_json_output(['data' => [], 'error' => FALSE, 'source' => 'mview']);
    return;
  }

  drupal_json_output([
    'data' => $results,
    'error' => FALSE,
    'source' => 'mview',
  ]);
}

/**
 * Perform search by field.
 *
 * @param $field
 * @param $value
 *
 * @return bool|\DatabaseStatementInterface
 */
function tripal_analysis_expression_search_features($field, $value) {
  $sql = "SELECT DISTINCT EF.feature_uniquename AS uniquename,
                 EF.organism_common_name AS common_name,
                 EF.accession AS accession 
            FROM {expression_feature} EF
            WHERE LOWER(EF.$field) LIKE :$field
            ORDER BY $field
            LIMIT 20";

  $term = db_like(strtolower($value)) . '%';

  $results = chado_query($sql, [
    ":$field" => $term,
  ]);

  if ($results) {
    return $results->fetchAll();
  }

  return $results;
}
