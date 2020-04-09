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
  public static $field_types = array('local__expression_search');

  // The list of default settings for this formatter.
  public static $default_settings = array(
    'setting1' => 'default_value',
  );

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

    $query = chado_db_select('analysis', 'a');
    $query->distinct();
    $query->fields('f', ['organism_id']); //feature name and id for
    $query->fields('fc', ['cvterm_id']);
    $query->fields('ct', ['name']); // Keyword
    $query->join('analysisfeature', 'af', 'a.analysis_id = af.analysis_id');
    $query->join('feature', 'f', 'f.feature_id = af.feature_id');
    $query->join('feature_cvterm', 'fc', 'f.feature_id = fc.feature_id');
    $query->join('feature_cvtermprop', 'fp', 'fc.feature_cvterm_id = fp.feature_cvterm_id AND fp.type_id = :typeid', [':typeid' => $pvalue_cvterm_id]);
    $query->join('feature_cvtermprop', 'fp2', 'fp.feature_cvterm_id = fp2.feature_cvterm_id AND fp2.type_id = :typeid2', [':typeid2' => $evidence_code_cvterm_id]);
    $query->join('cvterm', 'ct', 'fc.cvterm_id = ct.cvterm_id');
    #$query->addExpression('COUNT(*)', 'count');
    #$query->condition('a.analysis_id', $analysis_id, '='); // commented out for testing so I have multiple CVterms
    $total = $query->countQuery()->execute()->fetchField();
    $page = pager_default_initialize($total, 10);
    $query->range($page * 10, 10);

    $cvterms = $query->execute()->fetchAll();

    $cvterm_count = $this->getCount($cvterms, $analysis_id);
    foreach ($cvterms as $record) {
      $rows[] = [
        l($record->name, 'tripal_analysis_expression/pvalue/cvterm/' . $record->cvterm_id . '/' . $record->organism_id),
        $cvterm_count[$record->cvterm_id]->count ?? 0,
      ];
    }

    #foreach ($cvterm_count as $count) {
    #  $cvterm_count[$record['cvterm_id']]['count'];
    #  $indexed_authors[$count['author_id']]['books_count'] = $count['count'];
    #}

    $pager = theme('pager', ['quantity', $total]);
    $pager = $this->ajaxifyPager($pager, $entity);

    if ($rows == []) {
      $element[0] =
        [
          "#type" => "markup",
          "#markup" => "There are no keywords associated with p-values in this analysis.",
        ];
    } else {
      $table = theme('table',
        [
          "header" => ['Keyword', 'Count'],
          "rows" => $rows,
          'pager' => $pager,
        ]);

      $description = "View all keywords associated with p-values in this analysis, and the number of features with this keyword.";
      $element[0] =
        [
          "#type" => "markup",
          "#markup" => $description . $table . $pager,
        ];
    }
  }

  /**
   * @see ChadoFieldFormatter::settingsSummary()
   *
   **/

  protected function getCount($cvterms, $analysis_id)
  {
    $cvterm_ids = [];
    $indexed_cvterms = [];
    foreach ($cvterms as $cvterm) {
      $cvterm_ids[] = $cvterm->cvterm_id;
      $indexed_cvterms[$cvterm->cvterm_id] = $cvterm;
    }

    $count_query = db_select('chado.feature_cvterm', 'fc');
    $count_query->addExpression('COUNT(*)', 'count');
    $count_query->join('chado.analysisfeature', 'af', 'fc.feature_id = af.feature_id');
    $count_query->condition('fc.cvterm_id', $cvterm_ids, 'IN');
    $count_query->condition('af.analysis_id', $analysis_id);
    $count_query->fields('fc', ['cvterm_id']);
    $count_query->groupBy('fc.cvterm_id');
    $result = $count_query->execute();

    while ($record = $result->fetchObject()) {
      $indexed_cvterms[$record->cvterm_id]->count = $record->count;
    }

    return $indexed_cvterms;
  }

  // Loop through results to associate each cvterm with its count
  // End result will be [(cvterm_id, name, cvterm_count)]
  /*
   *
  // Eager load
  $author_count = "select author_id, count(*) from book where author_id in $author_ids group by author_id"

  foreach($author_count as $count) {
    $indexed_authors[$count['author_id]][books_count] = $count['count']
  }

   */
  public function settingsSummary($view_mode)
  {
    return '';
  }

}
