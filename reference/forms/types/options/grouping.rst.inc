grouping
~~~~~~~~

**type**: ``integer`` **default**: ``false``

Cette valeur est utilis�e en interne comme valeur de ``NumberFormatter::GROUPING_USED`` en utilisant la classe PHP ``NumberFormatter``.
Sa documentation est inexistante mais il apparait que si vous la d�finissez � ``true``,  les nombres seront regroup�s avec les virgules
ou par p�riode (selon votre locale) : ``12345.123`` affichera ``12,345.123``.