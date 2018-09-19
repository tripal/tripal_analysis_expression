<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class TripalBiomaterialLoaderTest extends TripalTestCase {
   use DBTransaction;




   public function test_biomat_loader_check_functionality(){
     module_load_include('inc', 'tripal_biomaterial', 'includes/TripalImporter/tripal_biomaterial_loader_v3');

     $importer = new \tripal_biomaterial_loader_v3();

     $organism = factory('chado.organism')->create();
     $analysis = factory('chado.analysis')->create();

     //before loading, insert some cvterms that will match the properties and values

     $cv_name = 'TAE_TEST_SUITE';
     $cv = factory('chado.cv')->create(['name' => $cv_name]);

     $prop_term = factory('chado.cvterm')->create(['name' => 'city', 'cv_id' => $cv->cv_id]);
     $value_term = factory('chado.cvterm')->create(['name' => 'blood', 'cv_id' => $cv->cv_id]);

     $file_path = __DIR__ .'/../example_files/biomaterials_example.xml';
     $output = tripal_biomaterial_test_biosample_cvterms_xml($file_path, $organism->organism_id, $analysis->analysis_id);

     $this->assertNotFalse($output);
     $this->assertArrayHasKey('attributes', $output);
     $this->assertArrayHasKey('values', $output);

     //TODO: The form manually deals with this stuff, and it does it 2x, once for properties and once for values.  it should be refactored.

     $fields = $output["attributes"];
     $this->assertArrayHasKey('camera', $fields);
     $this->assertArrayHasKey('city', $fields);

     $property_values = $output["values"];
   }



  /**
   * @group biomaterial
   * @group importer
   */
   public function test_biomats_are_loaded(){

     $organism = factory('chado.organism')->create();
     $analysis = factory('chado.analysis')->create();

     $file = ['file_local' => __DIR__ .'/../example_files/biomaterials_example.xml'];

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

}
