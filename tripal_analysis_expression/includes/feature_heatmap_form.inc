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

/**
 * Validate the settings form.
 *
 * @param $form
 * @param $form_state
 */
function tripal_analysis_expression_heatmap_settings_form_validate($form, &$form_state) {
  $use_elasticsearch = $form_state['values']['use_elasticsearch'];

  if ($use_elasticsearch && !module_exists('tripal_elasticsearch')) {
    form_set_error('use_elasticsearch', 'Tripal Elasticsearch module is not
                                         installed. Please install and enable it
                                         first before attempting to enable
                                         Elasticsearch integration.');
  }

  if ($use_elasticsearch && module_exists('tripal_elasticsearch')) {
    try {
      $es = new ESInstance();
      $indices = $es->getIndices();
      if (!in_array('gene_search_index', $indices)) {
        form_set_error('use_elasticsearch', 'Gene Search Index does not exist.
                                              Please create the index using the
                                              Elasticsearch admin pages and try
                                              again.');
      }
    } catch (Exception $exception) {
      form_set_error('use_elasticsearch', 'Elasticsearch error: '.$exception->getMessage());
    }
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
function tripal_analysis_expression_heatmap_settings_form_submit($form, &$form_state) {
  $placeholder = $form_state['values']['placeholder'];
  $example = $form_state['values']['example'];
  $use_elasticsearch = $form_state['values']['use_elasticsearch'];

  variable_set('tripal_analysis_expression_heatmap_placeholder', $placeholder);
  variable_set('tripal_analysis_expression_heatmap_example', $example);
  variable_set('analysis_expression_heatmap_db', $use_elasticsearch ? 'elasticsearch' : 'mview');

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
    '#markup' => t('<b>Feature unique names need to be separated by commas.</b>'),
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

  // Escape get value if it exists
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

/**
 * Create a search form.
 *
 * @param $form
 * @param $form_state
 *
 * @throws \Exception
 * @return mixed
 */
function feature_heatmap_search_form($form, &$form_state) {
  $default_organism = ['' => 'Any Organism'];
  $organisms = $default_organism + feature_heatmap_get_organisms();


  $form['organism'] = [
    '#type' => 'select',
    '#title' => t('Organism'),
    '#description' => t('Filter by species'),
    '#options' => $organisms,
    '#attributes' => [
      'id' => 'heatmap-search-organism',
      'class' => ['form-control'],
    ],
  ];

  $form['search_term'] = [
    '#type' => 'textfield',
    '#title' => t('Search Features'),
    '#description' => 'Search by gene name, annotation or blast description.',
    '#attributes' => [
      'placeholder' => t('Example: Kinase, GO:0008343'),
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
 *
 * @throws \Exception
 */
function tripal_analysis_expression_search_features_callback() {
  $organism = isset($_GET['organism']) ? $_GET['organism'] : NULL;
  $terms = isset($_GET['terms']) ? $_GET['terms'] : NULL;

  if (empty($terms) && empty($organism)) {
    drupal_json_output(['data' => []]);
    return;
  }

  $db = variable_get('analysis_expression_heatmap_db');

  if ($db === 'elasticsearch') {
    tripal_analysis_expression_search_features_es();
    return;
  }

  tripal_analysis_expression_search_features_mview();
}

/**
 * Perform search using elasticsearch
 *
 * @throws \Exception
 */
function tripal_analysis_expression_search_features_es() {
  $terms = isset($_GET['terms']) ? $_GET['terms'] : '';
  $organism = isset($_GET['organism']) ? $_GET['organism'] : '';

  // Connect to ES
  try {
    $es = new ESInstance();
  } catch (Exception $exception) {
    // Fallback to use the mview since ES failed to connect.
    tripal_analysis_expression_search_features_mview(FALSE);
    return;
  }

  try {
    $query = tripal_elasticsearch_gene_search_index_query_mapper([
      'search_term' => '*'.$terms.'*',
      'organism' => $organism,
    ]);
    $index = 'gene_search_index';
    $results = $es->setTableSearchParams($index, '', $query, [0, 20])
      ->search();

    $data = array_map(function ($result) {
      elasticsearch_recursive_flatten($result['annotations'], $annotations);
      return [
        'uniquename' => $result['uniquename'],
        'common_name' => $result['organism_common_name'],
        'accession' => implode(',',$annotations),
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
 *
 * @throws \Exception
 */
function tripal_analysis_expression_search_features_mview($support_organism = TRUE) {
  $terms = isset($_GET['terms']) ? $_GET['terms'] : NULL;

  $results = tripal_analysis_expression_search_features('accession', $terms, $support_organism);

  if (!$results) {
    $results = tripal_analysis_expression_search_features('feature_uniquename', $terms, $support_organism);
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
 * @throws \Exception
 * @return bool|\DatabaseStatementInterface
 */
function tripal_analysis_expression_search_features($field, $value, $support_organism = TRUE) {
  $organism = isset($_GET['organism']) ? $_GET['organism'] : NULL;

  $query = db_select('chado.expression_feature', 'EF');
  $query->addField('EF', 'feature_uniquename', 'uniquename');
  $query->addField('EF', 'organism_common_name', 'common_name');
  $query->addField('EF', 'accession', 'accession');
  $query->orderBy($field, 'ASC');
  $query->range(0, 20);

  if (!empty($value)) {
    $query->condition($field, '%' . db_like($value) . '%', 'LIKE');
  }

  if (!empty($organism) && $support_organism) {
    $query->condition('organism_id', $organism);
  }

  $results = $query->execute();

  if ($results) {
    return $results->fetchAll();
  }

  return FALSE;
}

/**
 * Get a list of organisms for the heat map search.
 *
 * @return array
 * @throws \Exception
 */
function feature_heatmap_get_organisms() {
  $db = variable_get('analysis_expression_heatmap_db');

  if ($db === 'elasticsearch') {
    return drupal_map_assoc(tripal_elasticsearch_get_gene_search_organisms());
  }

  // subquery here is faster than a join
  $sql = 'SELECT * FROM {organism} WHERE organism_id IN
    (SELECT DISTINCT organism_id FROM {expression_feature})
    ORDER BY genus ASC, species ASC';

  $organism_list = chado_query($sql)->fetchAll();

  $organisms = [];
  foreach ($organism_list as $organism) {
    $organisms[$organism->organism_id] = chado_get_organism_scientific_name($organism);
    if (!empty($organism->common_name)) {
      $organisms[$organism->organism_id] .= " ($organism->common_name)";
    }
  }

  // if the site uses the chado infraspecific nomenclature, organisms will not be fully
  // alphabetically sorted by subspecies names, but we can't include in the SQL easily
  // because the columns may not be present, so sort the list again by scientific name
  natcasesort($organisms);

  return $organisms;
}
