<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__expression_search_formatter extends ChadoFieldFormatter
{

  // The default label for this field.
  public static $default_label = 'Analysis Keywords';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['local__expression_search'];

  // The list of default settings for this formatter.
  public static $default_settings = [
    'setting1' => 'default_value',
  ];

  /**
   * @see ChadoFieldFormatter::settingsForm()
   *
   **/

  public function settingsForm($view_mode, $form, &$form_state)
  {

  }

  /**
   * @see ChadoFieldFormatter::View()
   *
   **/

  public function view(&$element, $entity_type, $entity, $langcode, $items, $display)
  {
    $pvalue_cvterm_id = chado_get_cvterm([
      'cv_id' => ['name' => 'OBI'],
      'name' => 'p-value',
    ])->cvterm_id;
    $evidence_code_cvterm_id = chado_get_cvterm([
      'cv_id' => ['name' => 'local'],
      'name' => 'evidence code',
    ])->cvterm_id;
    $analysis_id = $entity->chado_record->analysis_id;

    $sql = "
    SELECT ct.cvterm_id, ct.name, COUNT(f.feature_id) as count FROM chado.analysis a
    INNER JOIN chado.analysisfeature af ON a.analysis_id = af.analysis_id
    INNER JOIN chado.feature f ON f.feature_id = af.feature_id
    INNER JOIN chado.feature_cvterm fc ON f.feature_id = fc.feature_id
    INNER JOIN chado.feature_cvtermprop fp ON fc.feature_cvterm_id = fp.feature_cvterm_id AND fp.type_id = :typeid
    INNER JOIN chado.feature_cvtermprop fp2 ON fp.feature_cvterm_id = fp2.feature_cvterm_id AND fp2.type_id = :typeid2
    INNER JOIN chado.cvterm ct ON fc.cvterm_id = ct.cvterm_id
    WHERE a.analysis_id = :analysis_id
    GROUP BY ct.cvterm_id
    ";
    $cvterms = db_query($sql, [
      ':typeid' => $pvalue_cvterm_id,
      ':typeid2' => $evidence_code_cvterm_id,
      ':analysis_id' => $analysis_id,
    ])->fetchAll();

    $organism_query = "SELECT * FROM chado.analysis_organism WHERE analysis_id=:id";
    $organism = db_query($organism_query, [':id' => $analysis_id])->fetchObject();

    $description = "View all keywords associated with p-values in this analysis, and the number of features with this keyword.";
    if (empty($cvterms)) {
      return $element[0] = [
        '#markup' => "<div>$description</div><div class=\"text-muted mt-2\">No keywords found for this analysis.</div>",
      ];
    }
    foreach ($cvterms as $record) {
      $rows[] = [
        $organism ? l($record->name,
          'tripal_analysis_expression/pvalue/cvterm/' . $record->cvterm_id . '/' . $organism->organism_id) : $record->name,
        $record->count,
      ];
    }
    if ($rows == []) {
      $element[0] = [
        "#type" => "markup",
        "#markup" => "There are no keywords associated with p-values in this analysis.",
      ];
    } else {
      $table = theme('table', [
        "header" => ['Keyword', 'Gene Count'],
        "rows" => $rows,
      ]);

      $element[0] = [
        "#type" => "markup",
        "#markup" => $description . $table,//. $pager,
      ];
    }
  }

  /**
   * @see ChadoFieldFormatter::settingsSummary()
   *
   **/
  
  public function settingsSummary($view_mode)
  {
    return '';
  }
}
