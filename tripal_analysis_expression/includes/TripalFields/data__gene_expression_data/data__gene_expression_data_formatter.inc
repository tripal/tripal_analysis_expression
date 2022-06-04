<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class data__gene_expression_data_formatter extends ChadoFieldFormatter{

  // The default label for this field.
  public static $default_label = 'Expression Data';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['data__gene_expression_data'];

  // The list of default settings for this formatter.
  public static $default_settings = [
    'setting1' => 'default_value',
  ];

  /**
   * Provides the field's setting form.
   */
  public function settingsForm($view_mode, $form, &$form_state) {

  }
    

  /**
   *  Provides the display for a field 
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $record = $entity->chado_record;
    $chado_table = $entity->chado_table;
    $field_name = $this->field['field_name'];
        
    // Get the list of analyses
    $analyses = [];

    if (count($items) == 0 or (count($items) == 1 and empty($items[0]['value']))) {
      return;
    }
                  
    // Organize the items by analysis id
    $analyses = [];
    
    // Iterate through the analyses.
    foreach ($items as $item) {
      $item = $item['value'];
      $entity_id = preg_replace('/TripalEntity:/','', $item['operation:2945']['entity']);
      $aname = $item['operation:2945']['schema:name'];
      $data_link = $item['schema:DataDownload'];
      
      // Iterate through the descriptive statistics
      foreach ($item['AFP:0003636'] as $stat) {
        $type = $stat['NCIT:C25284'];
        $quantity = $stat['NCIT:C25463'];
        $dlink = '';
        if ($chado_table == 'feature') { 
          $dlink = "<a href=\"$data_link\" id=\"#expressionDownloadLink\">Feature Data</a>";
        }
        $analyses[$entity_id][$type] = [
          l($aname, 'bio_data/' . $entity_id, ['attributes' => ['target' => '_blank']]),
          $quantity > 1 ? $type . 's' : $type ,
          number_format($quantity), 
          $dlink,
        ];
        if ($chado_table == 'analysis') {
          array_shift($analyses[$entity_id][$type]);
          array_pop($analyses[$entity_id][$type]);
        }
      }
    }

    $rows = [];    
    foreach ($analyses as $entity_id => $types) {
      foreach ($types as $row) { 
        $rows[] = $row;
      }
    }

    $headers = ['Analysis', 'Element', 'Count', 'Download'];
    if ($chado_table == 'analysis') {
      array_shift($headers);
      array_pop($headers);
    }
    $table = theme_table([
      'header' => $headers,
      'rows' => $rows,
      'attributes' => ['class' => ['table', 'tripal-analysis-expression-summary-table']],
      'caption' => $chado_table == 'analysis' ? 'Summary stats for this analysis.' : 'Summary stats for all analyses with this feature.',
      'colgroups' => [],
      'sticky' => FALSE,
      'empty' => 'There are no values'
    ]);   
      
    $content = [];
    $content['data_summary_table'] = [
      '#type' => 'item',
      '#markup' => $table
    ];
    
    // Return the page.
    $element[0] =  [
      '#type' => 'markup',
      '#markup' => drupal_render($content),
    ];        
  }

  /**
   * Provides a summary of the formatter settings.
   */
  public function settingsSummary($view_mode) {
    return '';
  }
}
