<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

/**
 *
 */
class TripalBiomaterialLoaderTest extends TripalTestCase {
  use DBTransaction;

  /**
   * @group biomaterial
   * @group importer
   */
  public function test_biomats_are_loaded() {

    $organism = factory('chado.organism')->create();
    $analysis = factory('chado.analysis')->create();

    $file = ['file_local' => __DIR__ . '/../example_files/biomaterials_example.xml'];

    $run_args = [
      'organism_id' => $organism->organism_id,
      'analysis_id' => $analysis->analysis_id,
    ];

    module_load_include('inc', 'tripal_biomaterial', 'includes/TripalImporter/tripal_biomaterial_loader_v3');

    $importer = new \tripal_biomaterial_loader_v3();
    $importer->create($run_args, $file);
    $importer->prepareFiles();
    $importer->run();

    $results = db_select('chado.biomaterial', 'b')
      ->fields('b')
      ->condition('taxon_id', $organism->organism_id)
      ->execute()
      ->fetchAll();

    $this->assertNotFalse($results);
    $this->assertNotEmpty($results);
    $this->assertEquals(5, count($results));
  }

  /**
   * @group wip
   */
  public function test_parse_xml_biomaterial_file() {
    $file = __DIR__ . '/../example_files/biomaterials_example.xml';

    module_load_include('inc', 'tripal_biomaterial', 'includes/TripalImporter/tripal_biomaterial_loader_v3');

    $importer = reflect(new \tripal_biomaterial_loader_v3());

    $out = $importer->parse_xml_biomaterial_file($file);
    $fields = $out["attributes"];
    $this->assertArrayHasKey('camera', $fields);
    $this->assertArrayHasKey('city', $fields);
  }

  /**
   * @group csv
   * @group flat
   */
  public function testParseCSV() {

    // Drop biomaterial if it is pre-existing.
    $biomat = db_select('chado.biomaterial', 'b')->fields('b')->condition('b.name', 'SM1')->execute()->fetchObject();

    if ($biomat) {
      db_delete('chado.biomaterial')->condition('name', 'SM1');
    }

    module_load_include('inc', 'tripal_biomaterial', 'includes/TripalImporter/tripal_biomaterial_loader_v3');

    $importer = reflect(new \tripal_biomaterial_loader_v3());

    $file = __DIR__ . '/../example_files/exampleCSV.csv';

    $organism = factory('chado.organism')->create();
    $analysis = factory('chado.analysis')->create();

    $importer->analysis_id = $analysis->analysis_id;
    $importer->organism_id = $organism->organism_id;

    $importer->load_biosample_flat($file, [], []);

    $biomat = db_select('chado.biomaterial', 'b')->fields('b')->condition('b.name', 'SM1')->execute()->fetchObject();

    $this->assertNotFalse($biomat);
    $this->assertEquals('Sugar Maple - pooled seedling leaf RNAs from various stress treatments', $biomat->description);

    $file = __DIR__ . '/../example_files/3175-biomaterial.csv';
    $importer = reflect(new \tripal_biomaterial_loader_v3());
    $organism = factory('chado.organism')->create();
    $analysis = factory('chado.analysis')->create();

    $importer->analysis_id = $analysis->analysis_id;
    $importer->organism_id = $organism->organism_id;

    $importer->load_biosample_flat($file, [], []);

    $biomat = db_select('chado.biomaterial', 'b')->fields('b')->condition('b.name', 'R20T17')->execute()->fetchObject();

    $this->assertNotFalse($biomat);
    $this->assertEquals('HLB tolerant sample', $biomat->description);

  }




}
