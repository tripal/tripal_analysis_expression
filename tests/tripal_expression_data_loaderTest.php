<?php

namespace Tests;

use PHPUnit\Exception;
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
      'fileext' => NULL,
      'feature_uniquenames' => 'uniq',
      're_start' => NULL,
      're_stop' => NULL,
      'feature_uniquenames' => NULL,
      'quantificationunits' => NULL,
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

    $this->assertEquals(1, count($result));


    $query = db_select('chado.elementresult', 'er');
    $query->join('chado.element', 'e', 'e.element_id = er.element_id');
    $query->fields('er');
    $query->condition('e.feature_id', $features[0]->feature_id);
    $results = $query->execute()
      ->fetchAll();

    $this->assertNotFalse($results);
    $this->assertEquals(3, count($results));
  }


  /**
   * Expression loader threw a scar-sounding error that it was overwriting
   * biomaterials if they were already loaded.  Make sure that we dont lose the
   * biomat properties.
   *
   * @ticket 232
   *
   * @throws \Exception
   */
  public function test_expression_loader_doesnt_overwite_biomat_properties() {


    $organism = factory('chado.organism')->create();
    $analysis = factory('chado.analysis')->create();

    $this->load_biomaterials($organism, $analysis);
    //create expected features

    $features = $this->create_features($organism, NULL);

    $biomat_name = 'art';

    $original_biomat = db_select('chado.biomaterial', 'b')
      ->fields('b')
      ->condition('name', $biomat_name)
      ->execute()
      ->fetchObject();


    module_load_include('inc', 'tripal_analysis_expression', 'includes/TripalImporter/tripal_expression_data_loader');
    $file = ['file_local' => __DIR__ . '/../example_files/example_expression.tsv'];


    $run_args = [
      'filetype' => 'mat', //matrix file type
      'organism_id' => $organism->organism_id,
      'analysis_id' => $analysis->analysis_id,
      //optional
      'fileext' => NULL,
      'feature_uniquenames' => 'uniq',
      're_start' => NULL,
      're_stop' => NULL,
      'feature_uniquenames' => NULL,
      'quantificationunits' => NULL,
    ];


    $importer = new \tripal_expression_data_loader();
    $importer->create($run_args, $file);
    $importer->prepareFiles();
    $importer->run();

    //first check every column of the biomaterial.  it shouldnt have changed.
    $biomat_post = db_select('chado.biomaterial', 'b')
      ->condition('b.name', $biomat_name)
      ->fields('b')
      ->execute()
      ->fetchObject();

    $keys = ['taxon_id', 'biosourceprovider_id', 'dbxref_id', 'name', 'description'];

    foreach ($keys as $key){
      $this->assertObjectHasAttribute($key, $biomat_post);
      $this->assertEquals($original_biomat->$key, $biomat_post->$key, "Biomaterial key $key was changed by expression loader.\n  {$original_biomat->$key} => {$biomat_post->$key}.");
    }

    //Check properties
    $query = db_select('chado.biomaterialprop', 'bp');
    $query->join('chado.biomaterial', 'b', 'b.biomaterial_id = bp.biomaterial_id');
    $query->fields('bp', ['value']);
    $query->condition('b.name', $biomat_name);
    $query->condition('bp.value', 'husband');
    $prop = $query->execute()->fetchField();

    $this->assertEquals('husband', $prop);

  }


  /**
   * @group wip
   */
  public function test_type_required_submethod() {

    $organism = factory('chado.organism')->create();
    $analysis = factory('chado.analysis')->create();

    // $this->load_biomaterials($organism, $analysis);
    //create teh expected features

    $so = chado_get_cv(['name' => 'sequence']);

    $termA = factory('chado.cvterm')->create(['cv_id' => $so->cv_id]);
    $termB = factory('chado.cvterm')->create(['cv_id' => $so->cv_id]);


    $featuresA = $this->create_features($organism, $termA);
    $featuresB = $this->create_features($organism, $termB);


    module_load_include('inc', 'tripal_analysis_expression', 'includes/TripalImporter/tripal_expression_data_loader');


    $importer = new \tripal_expression_data_loader();

    $private = reflect($importer);

    $feature = $private->tripal_expression_find_feature_id(
      $featuresA[0]->name,
      $organism->organism_id,
      $feature_uniquenames = FALSE,
      $type_id = NULL);

    $this->assertFalse($feature);
  }

  /**
   * Test that if two features have the same name, we can specify the type_id
   * to successfully load expression data.
   *
   */
  public function test_specifying_type_allows_name_overlap_loading() {


    $organism = factory('chado.organism')->create();
    $analysis = factory('chado.analysis')->create();

    $this->load_biomaterials($organism, $analysis);
    //create teh expected features

    $so = chado_get_cv(['name' => 'sequence']);

    $termA = factory('chado.cvterm')->create(['cv_id' => $so->cv_id]);
    $termB = factory('chado.cvterm')->create(['cv_id' => $so->cv_id]);


    $featuresA = $this->create_features($organism, $termA);
    $featuresB = $this->create_features($organism, $termB);


    module_load_include('inc', 'tripal_analysis_expression', 'includes/TripalImporter/tripal_expression_data_loader');
    $file = ['file_local' => __DIR__ . '/../example_files/example_expression.tsv'];


    $run_args = [
      'filetype' => 'mat', //matrix file type
      'organism_id' => $organism->organism_id,
      'analysis_id' => $analysis->analysis_id,
      'seqtype' => $termA->name,
      //optional
      'type' => NULL,
      'fileext' => NULL,
      'feature_uniquenames' => 'uniq',
      're_start' => NULL,
      're_stop' => NULL,
      'feature_uniquenames' => NULL,
      'quantificationunits' => NULL,
    ];

    $importer = new \tripal_expression_data_loader();
    $importer->create($run_args, $file);
    $importer->prepareFiles();
    $importer->run();


    $query = db_select('chado.elementresult', 'er');
    $query->join('chado.element', 'e', 'e.element_id = er.element_id');
    $query->fields('er');
    $query->condition('e.feature_id',
      $featuresA[0]->feature_id
    );
    $results = $query->execute()
      ->fetchAll();

    $this->assertNotEmpty($results);
    $this->assertEquals(3, count($results));

  }
}
