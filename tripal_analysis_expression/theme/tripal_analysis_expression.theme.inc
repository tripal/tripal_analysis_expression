<?php

function tripal_analysis_expression_preprocess_tripal_feature_expression(&$variables) {
  $feature = $variables['node']->feature;
  $sql = "SELECT DISTINCT(AN.analysis_id), AN.name
              FROM {elementresult} ER
              INNER JOIN {element} E ON E.element_id = ER.element_id
              INNER JOIN {feature} F ON F.feature_id = E.feature_id
              INNER JOIN {quantification} Q ON Q.quantification_id = ER.quantification_id
              INNER JOIN {acquisition} AQ ON AQ.acquisition_id = Q.acquisition_id
              INNER JOIN {assay} A ON A.assay_id = AQ.assay_id
              INNER JOIN {assay_biomaterial} AB ON AB.assay_id = A.assay_id
              INNER JOIN {biomaterial} B ON B.biomaterial_id = AB.biomaterial_id
              INNER JOIN {analysis} AN ON AN.analysis_id = Q.analysis_id
              WHERE F.feature_id = :feature_id";
  $analyses = chado_query($sql, [":feature_id" => $feature->feature_id]);
  if ($analyses) {
    $analyses = $analyses->fetchAll();
    foreach ($analyses as $key => $analysis) {
      $analyses[$key]->url = 'bio_data/' . chado_get_record_entity_by_table('analysis',
          $analysis->analysis_id);
    }
    $variables['analyses'] = $analyses;
  }
}