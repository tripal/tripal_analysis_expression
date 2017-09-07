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

  $form['heatmap_feature_uniquename'] = [
    '#type' => 'textarea',
    '#title' => t('Enter feature unique names'),
    '#description' => t('<b>Example feature unique names:</b> Fraxinus_pennsylvanica_120313_comp59663_c0_seq1, Fraxinus_pennsylvanica_120313_comp59663_c0_seq2, Fraxinus_pennsylvanica_120313_comp56723_c0_seq1, Fraxinus_pennsylvanica_120313_comp60325_c0_seq16, Fraxinus_pennsylvanica_120313_comp61194_c0_seq4, Fraxinus_pennsylvanica_120313_comp61194_c0_seq5, Fraxinus_pennsylvanica_120313_comp61194_c0_seq8, Fraxinus_pennsylvanica_120313_comp61194_c0_seq6'),
    '#attributes' => [
      'placeholder' => 'Example: Fraxinus_pennsylvanica_120313_comp59663_c0_seq1, Fraxinus_pennsylvanica_120313_comp59663_c0_seq2, Fraxinus_pennsylvanica_120313_comp56723_c0_seq1, Fraxinus_pennsylvanica_120313_comp60325_c0_seq16, Fraxinus_pennsylvanica_120313_comp61194_c0_seq4, Fraxinus_pennsylvanica_120313_comp61194_c0_seq5, Fraxinus_pennsylvanica_120313_comp61194_c0_seq8, Fraxinus_pennsylvanica_120313_comp61194_c0_seq6',
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

function generate_heatmap_values() {
  // Escaped get value if it exists
  if (!isset($_GET['heatmap_feature_uniquename']) || empty($_GET['heatmap_feature_uniquename'])) {
    return NULL;
  }

  $heatmap_feature_uniquename = e($_GET['heatmap_feature_uniquename']);

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

  $expressions = [];

  if (!empty($heatmap_feature_uniquename)) {  // search with feature uniquename
    $feature_ids = preg_split("/[\s,]+/", $heatmap_feature_uniquename, -1, PREG_SPLIT_NO_EMPTY);
    foreach ($feature_ids as $feature_id) {
      // trim white space at the beginning and the end
      $feature_id = trim($feature_id);
      $expressions[$feature_id] = db_query($sql_feature_uniquename, [":feature_uniquename" => $feature_id])->fetchAllAssoc('elementresult_id');
    }
  }

  if (!empty($expressions)) {
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
    foreach ($expressions as $feature_id => $features) {
      foreach ($biomaterials as $biomaterial_id) {
        $expression_data[$feature_id][$biomaterial_id] = NULL;
      }
    }

    // Foreach feature, loop through the biomaterials it has and get the signal value (expression data)
    foreach ($expressions as $feature_id => $features) {
      foreach ($features as $feature) {

        // get analysis URL
        $analysis_url = '';
        $analysis_id = $feature->analysis_id;
        $sql_analysis = "SELECT * FROM chado.analysis WHERE analysis_id=:analysis_id";
        if (!empty($analysis_id)) {
          $analysis = db_query($sql_analysis, [':analysis_id' => $analysis_id])->fetchAllAssoc('analysis_id');
          // analysis page url consists of analysis name, sourcename, program and programversion
          $analysis_url = $analysis[$analysis_id]->name . ', ' . $analysis[$analysis_id]->sourcename . ' (' . $analysis[$analysis_id]->program . ', ' . $analysis[$analysis_id]->programversion . ')';
          $analysis_url = preg_replace("/[^A-Za-z0-9 ]/", '', $analysis_url); // remove all non-alphanumeric charaters
          $analysis_url = preg_replace("/\s/", '-', $analysis_url); // replace ' ' with '-'
          $analysis_url = 'content/' . strtolower($analysis_url);
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
      if (strlen($feature_id) > $max_id_len) {
        $max_id_len = strlen($feature_id);
      }
      $y[] = '<a href="/feature/' . $feature_id . '">' . $feature_id . '</a>';
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
    $left_margin = $max_id_len * 8;
    // use the max length of treatment name times 7 to get an appropriate pixel number of bottom margin
    $bottom_margin = $max_treatment_name * 9;

    // Build variable for plotly function in js file.
    // Note that this variable requires that the x, y, z and type are not within double quotations
    // To build this variable, we first create a string, and then use javascripts' eval() function
    // to convert this string into a javascript variable.
    // The eval() function is called in the heatmap.js file.
    if (!empty($x) and !empty($y) and !empty($x)) {
      $heatmap_data = '[{' . '
                                      x:' . json_encode($x) . ', 
                                      y:' . json_encode($y) . ', 
                                      z:' . json_encode($z) . ', 
                                      text:' . json_encode($text) . ',
                                      hoverinfo: "all",
                                      type:"heatmap"}]';
    }
    else {
      $heatmap_data = '';
    }

    return [
      'heatmap_data' => $heatmap_data,
      'bottom_margin' => $bottom_margin,
      'left_margin' => $left_margin,
    ];
  }

  return NULL;
}