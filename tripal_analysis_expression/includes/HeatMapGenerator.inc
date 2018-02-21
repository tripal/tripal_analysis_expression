<?php

class HeatMapGenerator {

  /**
   * GET feature names.
   *
   * @var array
   */
  protected $featureNames = [];

  /**
   * Holds a set of biomaterial props indexed by name.
   *
   * @var array
   */
  protected $biomaterialProps = [];

  public function __construct() {
    if (!isset($_GET['heatmap_feature_uniquename']) || empty($_GET['heatmap_feature_uniquename'])) {
      return;
    }

    // Obtain unique names
    $this->featureNames = array_map('trim', explode(',', $_GET['heatmap_feature_uniquename']));
  }

  /**
   * Generate heat map data.
   *
   * @throws \Exception
   * @return object|bool
   */
  public function generate() {
    if (count($this->featureNames) < 2) {
      return FALSE;
    }

    $expression = $this->getExpressionData();

    // Eager load biomaterial props
    foreach ($expression->biomaterials as $analysis_id => $biomaterial) {
      $this->biomaterialProps[$analysis_id] = [];
      $this->loadBiomaterialProps($expression->biomaterials[$analysis_id], $analysis_id);
    }

    // Set URLs
    // The number of features is usually very small so looping directly here
    // is not a big deal
    foreach ($expression->features as $analysis_id => $features) {
      foreach ($features as $feature_id => $feature) {
        $expression->features[$analysis_id][$feature_id] = l($feature, 'bio_data/' . chado_get_record_entity_by_table('feature', $feature_id));
      }
    }

    // Attach props array to returned object
    $expression->properties = $this->biomaterialProps;

    return $expression;
  }

  /**
   * Get features from the DB.
   *
   * @throws \Exception
   * @return object
   */
  protected function getExpressionData() {
    $sql = "SELECT ER.signal,
                   F.feature_id AS feature_id,
                   F.uniquename AS feature_uniquename,
                   B.name AS biomaterial_name,
                   B.biomaterial_id,
                   B.description AS biomaterial_description,
                   AN.analysis_id,
                   AN.name AS analysis_name,
                   C.name AS contact_name
            FROM {elementresult} ER
            INNER JOIN {element} E ON E.element_id = ER.element_id
            INNER JOIN {feature} F ON F.feature_id = E.feature_id
            INNER JOIN {quantification} Q ON Q.quantification_id = ER.quantification_id
            INNER JOIN {acquisition} AQ ON AQ.acquisition_id = Q.acquisition_id
            INNER JOIN {assay} A ON A.assay_id = AQ.assay_id
            INNER JOIN {assay_biomaterial} AB ON AB.assay_id = A.assay_id
            INNER JOIN {biomaterial} B ON B.biomaterial_id = AB.biomaterial_id
            INNER JOIN {analysis} AN ON AN.analysis_id = Q.analysis_id
            LEFT JOIN {contact} C ON B.biosourceprovider_id = C.contact_id
            WHERE F.uniquename IN (:uniquenames)";

    $expressionData = chado_query($sql, [
      ':uniquenames' => $this->featureNames,
    ])->fetchAll();

    $data = [];
    $biomaterials = [];
    $analyses = [];
    $features = [];
    $max_feature_len = 0;
    $max_biomaterial_len = 0;

    foreach ($expressionData as $expression) {
      $analyses[$expression->analysis_id] = $expression->analysis_name;
      $biomaterials[$expression->analysis_id][$expression->biomaterial_id] = [
        'name' => $expression->biomaterial_name,
        'contact' => $expression->contact_name ?: 'Not Set',
        'description' => wordwrap($expression->biomaterial_description, 75, '<br />'),
        'props' => [],
      ];
      $features[$expression->analysis_id][$expression->feature_id] = $expression->feature_uniquename;
      $data[$expression->analysis_id][$expression->feature_id][$expression->biomaterial_id] = $expression->signal;

      // Compute the longest string
      $biomaterial_len = strlen($expression->biomaterial_name);
      if ($biomaterial_len > $max_biomaterial_len) {
        $max_biomaterial_len = $biomaterial_len;
      }

      $feature_len = strlen($expression->feature_uniquename);
      if ($feature_len > $max_feature_len) {
        $max_feature_len = $feature_len;
      }
    }

    return (object) [
      'analyses' => $analyses,
      'biomaterials' => $biomaterials,
      'features' => $features,
      'data' => $data,
      'maxLengths' => [
        'feature' => $max_feature_len,
        'biomaterial' => $max_biomaterial_len,
      ],
    ];
  }

  /**
   * Get biomaterial props.
   *
   * @param array $biomaterials Array of biomaterials (index by biomaterial id)
   * @param int $analysis_id Analysis ID
   *
   * @throws \Exception
   */
  protected function loadBiomaterialProps(array &$biomaterials, $analysis_id) {
    $sql = "SELECT CVT.name AS name,
                   BP.value AS value,
                   BP.biomaterialprop_id,
                   B.biomaterial_id AS biomaterial_id
            FROM {biomaterial} B
            INNER JOIN {biomaterialprop} BP ON B.biomaterial_id = BP.biomaterial_id
            INNER JOIN {cvterm} CVT ON BP.type_id = CVT.cvterm_id
            WHERE B.biomaterial_id IN (:ids)";
    $props = chado_query($sql, [':ids' => array_keys($biomaterials)])->fetchAll();

    foreach ($props as $prop) {
      $biomaterials[$prop->biomaterial_id]['props'][$prop->name] = [
        'name' => $prop->name,
        'value' => $prop->value,
      ];

      // A construct that holds a list of all the available properties for an
      // analysis
      $this->biomaterialProps[$analysis_id][$prop->name] = $prop->name;
    }
  }
}
