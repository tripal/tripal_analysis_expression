<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class tripal_expression_data_loaderTest extends TripalTestCase {

     use DBTransaction;

  private function load_biomaterials($organism, $analysis) {
    module_load_include('inc', 'tripal_biomaterial', 'includes/TripalImporter/tripal_biomaterial_loader_v3');
    $file = ['file_local' => __DIR__ . '/../example_files/biomaterials_example.xml'];

    $run_args = [
      'organism_id' => $organism->organism_id,
      'analysis_id' => $analysis->analysis_id,
    ];
    $importer = new \tripal_biomaterial_loader_v3();
    $importer->create($run_args, $file);
    $importer->prepareFiles();
    $importer->run();


  }

  /**
   * Creates features with or without a type specified
   *
   * @param null $term
   *
   * @return array
   */
  private function create_features($organism, $term = NULL) {

    $genes = ['TAE_gene_A', 'TAE_gene_B'];

    $out = [];
    foreach ($genes as $gene) {
      $params = [
        'name' => $gene,
        'uniquename' => $gene,
        'organism_id' => $organism->organism_id,
      ];
      if ($term) {
        $params['type_id'] = $term->cvterm_id;
      }
      $out[] = factory('chado.feature')->create($params);

    }
    return $out;

  }

  public function test_expression_loader_properly_inserts_into_element() {

    $organism = factory('chado.organism')->create();
    $analysis = factory('chado.analysis')->create();

    $this->load_biomaterials($organism, $analysis);
    //before loading, insert some cvterms that will match the properties and values

    //create teh expected features

    $features = $this->create_features($organism, NULL);


    module_load_include('inc', 'tripal_analysis_expression', 'includes/TripalImporter/tripal_expression_data_loader');
    $file = ['file_local' => __DIR__ . '/../example_files/example_expression.tsv'];


    $run_args = [
      'filetype' => 'mat', //matrix file type
      'organism_id' => $organism->organism_id,
      'analysis_id' => $analysis->analysis_id,
      //optional
      'fileext' => null,
      'feature_uniquenames' => 'uniq',
      're_start' => null,
      're_stop' => null,
      'feature_uniquenames' => null,
      'quantificationunits' => null,
    ];

    $importer = new \tripal_expression_data_loader();
    $importer->create($run_args, $file);
    $importer->prepareFiles();
    $importer->run();

    $result = db_select('chado.element', 'e')
      ->fields('e')
      ->condition('feature_id', $features[0]->feature_id)
      ->execute()
      ->fetchAll();

    //should be as many elements as listed for this feature
    $this->assertNotFalse($result);

    var_dump($result);

    $this->assertEquals(3, count($result));

  }
}
