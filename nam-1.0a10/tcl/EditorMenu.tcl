#-----------------------------------------------------------------------
# Editor instproc floatingToolbar {}
#   - Creates a floating tool pallete 
#-----------------------------------------------------------------------
Editor instproc floatingToolbar {} {
  $self instvar menulist_ SharedEnv

  set SharedEnv(Buttons) {
	  {RadioButton Select {
		set i [$SharedEnv(CurrentCanvas) create polygon -1.0 -1.0 24.0 -1.0 \
			24.0 -1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 171.0 \
			24.0 171.0 136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 \
			161.0 24.0 161.0 24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 \
			-1.0 24.0 -1.0 24.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create polygon 37.9541566013 \
			22.1761496591 61.9830972933 102.92577573 73.664874718 \
			69.3468499177 109.153797554 67.2150576124 \
			-fill $SharedEnv(ButtonColor) -outline #000000000000 -smooth 0 \
			-splinesteps 12 -stipple $SharedEnv(ButtonStipple1) -tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create polygon 73.664874718 \
			69.3468499177 113.417382164 138.192903283 129.140948917 \
			126.289330577 \
			-fill $SharedEnv(ButtonColor) -outline #000000000000 -smooth 0 \
			-splinesteps 12 -stipple $SharedEnv(ButtonStipple1) -tags {} -width 1]
	}}
  	{RadioButton Node {
		set i [$SharedEnv(CurrentCanvas) create polygon -1.0 -1.0 24.0 -1.0 \
			24.0 -1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 171.0 \
			24.0 171.0 136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 \
			161.0 24.0 161.0 24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 \
			-1.0 24.0 -1.0 24.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create polygon 83.0723583615 \
			21.1233836897 108.415159222 24.7089434403 130.701244279 \
			35.0331512701 147.242582766 50.850755345 156.044045139 \
			70.2539191706 156.044045139 90.9023348301 147.242582766 \
			110.305498656 130.701244279 126.123102731 108.415159222 \
			136.44731056 83.0723583615 140.032870311 57.7295575006 \
			136.44731056 35.4434724439 126.123102731 18.9021339567 \
			110.305498656 10.1006715836 90.9023348301 10.1006715836 \
			70.2539191706 18.9021339567 50.850755345 35.4434724439 \
			35.0331512701 57.7295575006 24.7089434403 \
			-fill $SharedEnv(ButtonColor) -outline black -smooth 1 \
			-splinesteps 12 -stipple $SharedEnv(ButtonStipple1) -tags {} -width 1]
	}}
    {RadioButton Link {
    set i [$SharedEnv(CurrentCanvas) create polygon \
      -0.852307692096 -0.935384615596 24.1476923079 -0.935384615596 \
      24.1476923079 -0.935384615596 174.221538462 -0.935384615596 \
      174.221538462 -0.935384615596 199.221538462 -0.935384615596 \
      199.221538462 24.0646153844 199.221538462 24.0646153844 \
      199.221538462 174.138461538 199.221538462 174.138461538 \
      199.221538462 199.138461538 174.221538462 199.138461538 \
      174.221538462 199.138461538 24.1476923079 199.138461538 \
      24.1476923079 199.138461538 -0.852307692096 199.138461538 \
      -0.852307692096 174.138461538 -0.852307692096 174.138461538 \
      -0.852307692096 24.0646153844 -0.852307692096 24.0646153844 -fill \
      {} -outline {} -smooth 0 -splinesteps 12 -stipple {} -tags {} \
      -width 1]
    set i [$SharedEnv(CurrentCanvas) create line 86.3401486816 \
      100.316868031 115.86971549 100.360304043 -arrow none -arrowshape \
      {8 10 3} -capstyle butt -fill black -joinstyle round -smooth 0 \
      -splinesteps 12 -stipple {} -tags {} -width 1]
    set i [$SharedEnv(CurrentCanvas) create line 145.312185136 \
      100.316868031 174.863526234 100.360304043 174.863526234 \
      70.9396457192 -arrow none -arrowshape {8 10 3} -capstyle butt \
      -fill black -joinstyle round -smooth 0 -splinesteps 12 -stipple \
      {} -tags {} -width 1]
    set i [$SharedEnv(CurrentCanvas) create line 174.798203363 \
      70.9070687107 174.863526234 70.9396457192 -arrow none -arrowshape \
      {8 10 3} -capstyle butt -fill $SharedEnv(ButtonColor) -joinstyle \
      round -smooth 0 -splinesteps 12 -stipple $SharedEnv(ButtonStipple2) \
      -tags {} -width 1]
    set SharedEnv(Smooth_Line) [$SharedEnv(CurrentCanvas) create line 25.0 \
      175.0 25.0092307692 100.036923077 100.036923077 100.036923077 \
      175.064615385 100.036923077 175.064615385 25.0092307692 -arrow \
      none -arrowshape {8 10 3} -capstyle butt -fill \
      $SharedEnv(ButtonColor) -joinstyle miter -smooth 0 -splinesteps 12 \
      -stipple $SharedEnv(ButtonStipple2) -tags {} -width 4]
    set i [$SharedEnv(CurrentCanvas) create line 25.0 125.0 \
      25.0092307692 100.036923077 50.0184615385 100.036923077 -arrow \
      none -arrowshape {8 10 3} -capstyle butt -fill black -joinstyle \
      miter -smooth 0 -splinesteps 12 -stipple {} -tags {} -width 1]
  }}
  	{RadioButton Queue {
		set i [$SharedEnv(CurrentCanvas) create polygon -1.0 -1.0 24.0 -1.0 24.0 \
			-1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 171.0 24.0 171.0 \
			136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 161.0 24.0 161.0 \
			24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 -1.0 24.0 -1.0 24.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create polygon 20.0 32.0 45.0 32.0 45.0 \
			32.0 126.0 32.0 126.0 32.0 151.0 32.0 151.0 57.0 151.0 57.0 151.0 \
			104.0 151.0 104.0 151.0 129.0 126.0 129.0 126.0 129.0 45.0 129.0 \
			45.0 129.0 20.0 129.0 20.0 104.0 20.0 104.0 20.0 57.0 20.0 57.0 \
			-fill $SharedEnv(ButtonColor) -outline black -smooth 0 \
			-splinesteps 12 -stipple $SharedEnv(ButtonStipple1) -tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create line 20.0 60.0 151.0 60.0 \
			-fill black \
			 -tags {} -width 2]
		set i [$SharedEnv(CurrentCanvas) create line 20.0 100.0 151.0 100.0 \
			-fill black \
			 -tags {} -width 2]
	}}
    {RadioButton Tracer {
    set i [$SharedEnv(CurrentCanvas) create polygon 0.0 0.0 \
      25.0 0.0 25.0 0.0 175.073846154 0.0 175.073846154 0.0 \
      200.073846154 0.0 200.073846154 25.0 200.073846154 25.0 \
      200.073846154 175.073846154 200.073846154 175.073846154 \
      200.073846154 200.073846154 175.073846154 200.073846154 \
      175.073846154 200.073846154 25.0 200.073846154 25.0 200.073846154 \
      0.0 200.073846154 0.0 175.073846154 0.0 175.073846154 0.0 25.0 \
      0.0 25.0 -fill {} -outline {} -smooth 0 -splinesteps 12 -stipple \
      {} -tags {} -width 1]
    set i [$SharedEnv(CurrentCanvas) create line 52.0184615385 \
      185.046153846 52.036923077 135.101538461 -arrow none -arrowshape \
      {8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
      -splinesteps 12 -stipple {} -tags {} -width 1]
    set i [$SharedEnv(CurrentCanvas) create line 68.0646153847 \
      53.036923077 118.101538462 53.0553846155 -arrow none -arrowshape \
      {8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
      -splinesteps 12 -stipple {} -tags {} -width 1]
    set i [$SharedEnv(CurrentCanvas) create line 52.0184615385 \
      116.046153846 52.036923077 66.101538461 -arrow none -arrowshape \
      {8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
      -splinesteps 12 -stipple {} -tags {} -width 1]
    set i [$SharedEnv(CurrentCanvas) create line 137.064615385 \
      53.036923077 187.101538462 53.0553846155 -arrow none -arrowshape \
      {8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
      -splinesteps 12 -stipple {} -tags {} -width 1]
    set i [$SharedEnv(CurrentCanvas) create polygon 27.0092307692 \
      177.009230769 27.0092307692 77.0092307692 77.0092307692 \
      27.0092307692 177.009230769 27.0092307692 177.009230769 \
      77.0092307692 77.0092307692 77.0092307692 77.0092307692 \
      177.009230769 -fill $SharedEnv(ButtonColor) -outline black -smooth 0 \
      -splinesteps 12 -stipple $SharedEnv(ButtonStipple1) -tags {} -width \
      1]
  }}
	{RadioButton Cut {
		set i [$SharedEnv(CurrentCanvas) create polygon -1.0 -2.0 \
			24.0 -2.0 24.0 -2.0 146.0 -2.0 146.0 -2.0 171.0 -2.0 171.0 23.0 \
			171.0 23.0 171.0 135.0 171.0 135.0 171.0 160.0 146.0 160.0 146.0 \
			160.0 24.0 160.0 24.0 160.0 -1.0 160.0 -1.0 135.0 -1.0 135.0 -1.0 \
			23.0 -1.0 23.0 -fill {} -outline {} -smooth 0 -splinesteps 12 \
			-stipple {} -tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create polygon \
			63.5388025603 6.9140133338 75.8988998203 77.453529439 \
			60.0668341612 70.8645988339 44.229360387 62.4943797334 \
			28.3270045365 53.8115600292 12.3008923946 46.2880351751 \
			16.6934038006 51.5511429489 21.7484124676 56.3004646424 \
			27.3362349823 60.6346877942 33.3273977037 64.6514344314 \
			39.5908844671 68.4476335052 45.9966330027 72.1213384293 \
			52.4146464512 75.7705546807 58.7146009345 79.493527403 \
			64.7661861443 83.3875800132 70.4395360295 87.5506221311 \
			75.604719127 92.080611307 80.1309673092 97.075206478 83.888414522 \
			102.632317268 86.7466850558 108.849315026 88.5755858388 \
			115.824349027 89.245120006 123.655424754 88.9390819701 \
			124.711246498 88.1899034817 125.887593214 85.0587164493 \
			126.96146656 81.9162149024 128.649961096 79.3727228435 \
			130.977178405 78.0363677075 133.967006319 78.2920498845 \
			140.948898406 81.1387372648 146.92000033 84.4369342348 \
			149.710915407 87.9357554379 151.056434961 96.8059144196 \
			146.65288348 97.7731426938 141.441699615 96.9262229088 \
			136.360148001 94.9145764852 131.356264886 92.3887367034 \
			126.377271678 90.0001253411 121.371562181 88.3979947574 \
			116.285472997 88.2334532841 111.066715874 90.1568898244 \
			105.663529798 92.6751898339 102.551502676 95.6331489542 \
			100.271569609 98.8543932576 98.94295938 102.163933398 \
			98.687533161 107.030055872 99.792335968 111.40010308 \
			101.998319533 115.425441998 104.907343727 119.257818444 \
			108.121902549 123.048795605 111.243712084 126.948590342 \
			113.873651536 131.108504223 115.613628705 135.680086862 \
			116.066281396 138.541342157 115.019730004 141.009415097 \
			113.517321311 142.827397836 111.585038474 143.738251727 \
			109.248960512 143.105776486 102.407352701 138.606045263 \
			97.510031144 135.679562378 96.215539786 132.537415684 \
			95.643488515 129.295139508  \
			95.794912928 126.06698727 96.669963826 124.073367001 \
			98.082694536 122.467101186 99.60452094 121.084832186 \
			101.270367238 119.761136598 103.11484801 90.480227109 \
			88.1028765011 88.2978844307 82.1691691271 86.838656662 \
			76.1832852353 81.4975130574 58.5542458718 76.9810262708 \
			40.4402866063 71.6667017213 22.8089161852 63.9311563203 \
			6.6264709607 63.5388025603 6.9140133338 -fill \
			$SharedEnv(ButtonColor) -outline black -smooth 0 -splinesteps 12 \
			-stipple $SharedEnv(ButtonStipple1) -tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create polygon \
			126.510482112 99.782778734 125.281930984 100.886679259 \
			124.333027471 102.199358547 123.745148065 103.666993989 \
			123.600632868 105.238379854 125.004350593 108.340225379 \
			127.15512505 110.754179219 129.757078443 112.432895488 \
			132.512569926 113.328658829 134.822988095 113.179832755 \
			137.065645556 112.38032825 138.962406678 111.055058193 \
			140.234813146 109.330833488 140.765639214 108.058702627 \
			141.196281095 107.296977514 139.950422302 104.259719468 \
			137.714152321 101.738945526 134.893578486 99.754387391 \
			131.896308071 98.328000582 126.510482112 99.782778734 -fill \
			$SharedEnv(DefButtonColor) -outline $SharedEnv(DefButtonColor) -smooth \
			0 -splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create polygon 85.124365772 \
			130.833572526 83.1687403834 132.206525778 81.6843201596 \
			133.939782386 80.754074319 135.965401489 80.4586971855 \
			138.217109414 81.5139673823 141.256536376 83.2077851804 \
			143.813722859 85.3504539501 145.804108261 87.7534145098 \
			147.142298379 89.3845585661 146.918126511 91.162689649 \
			146.110525232 92.7119136198 144.93551076 93.6563363455 \
			143.609099314 94.215893959 142.435508131 94.4235737431 \
			141.936833055 94.2390419018 138.807921963 93.3440933132 \
			135.463889179 91.5789640029 132.503870659 88.7857195863 \
			130.528832913 87.1583159762 130.204785312 85.124365772 \
			130.833572526 -fill $SharedEnv(DefButtonColor) -outline \
			$SharedEnv(DefButtonColor) -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
	}}
	{RadioButton Paste {
		set i [$SharedEnv(CurrentCanvas) create polygon 3.0 2.0 28.0 \
			2.0 28.0 2.0 152.0 2.0 152.0 2.0 177.0 2.0 177.0 27.0 177.0 27.0 \
			177.0 141.0 177.0 141.0 177.0 166.0 152.0 166.0 152.0 166.0 28.0 \
			166.0 28.0 166.0 3.0 166.0 3.0 141.0 3.0 141.0 3.0 27.0 3.0 27.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} -tags \
			{} -width 1]
		set i [$SharedEnv(CurrentCanvas) create polygon \
			67.9240825065 39.915838224 67.9240825065 35.0770838995 \
			67.9240825065 35.0770838995 71.7703623055 35.0770838995 \
			71.7703623055 35.0770838995 74.3345488381 20.5608209258 \
			82.0271084359 6.04455795217 85.8733882348 6.04455795217 \
			93.5659478326 20.5608209258 97.4122276315 35.0770838995 \
			99.9764141641 35.0770838995 99.9764141641 35.0770838995 \
			99.9764141641 39.915838224 67.9240825065 39.915838224 -fill \
			$SharedEnv(ButtonColor) -outline black -smooth 1 -splinesteps 12 \
			-stipple $SharedEnv(ButtonStipple2) -tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create line 69.2061757729 \
			36.0448347644 75.6417316834 37.0250919481 85.90226492 \
			37.0250919481 93.5976648475 37.0250919481 97.4453648113 \
			36.0569837599 -arrow none -arrowshape {8 10 3} -capstyle butt \
			-fill $SharedEnv(ButtonColor) -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create polygon \
			42.5939143386 73.5625787978 50.2306882263 64.5549266551 \
			65.5042360016 61.5523759408 69.3226229455 55.5472745123 \
			99.8697184961 55.5472745123 103.68810544 61.5523759408 \
			118.961653215 64.5549266551 126.598427103 73.5625787978 -fill \
			white -outline black -smooth 1 -splinesteps 12 -stipple \
			$SharedEnv(ButtonStipple2) -tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create polygon 66.744924848 \
			40.5334123072 66.744924848 40.5334123072 66.744924848 \
			52.5436151642 66.744924848 52.5436151642 84.5571027282 \
			55.5461658784 102.369280608 52.5436151642 102.369280608 \
			52.5436151642 102.369280608 40.5334123072 102.369280608 \
			40.5334123072 84.5571027282 43.5359630214 66.744924848 \
			40.5334123072 66.744924848 40.5334123072 -fill \
			$SharedEnv(ButtonColor) -outline black -smooth 1 -splinesteps 12 \
			-stipple $SharedEnv(ButtonStipple2) -tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create polygon \
			45.3703113914 70.5589194493 45.3703113914 70.5589194493 \
			45.3703113914 148.62523802 45.3703113914 148.62523802 \
			84.5571027279 151.627788734 123.743894064 148.62523802 \
			123.743894064 148.62523802 123.743894064 70.5589194493 \
			123.743894064 70.5589194493 80.9946671518 73.5614701636 \
			45.3703113914 70.5589194493 45.3703113914 70.5589194493 -fill \
			white -outline black -smooth 1 -splinesteps 12 -stipple \
			$SharedEnv(ButtonStipple2) -tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create polygon \
			52.5285150975 79.5654629582 52.5285150975 79.5654629582 \
			52.5285150975 139.616477243 52.5285150975 139.616477243 \
			86.0904001986 142.619027957 115.923186955 139.616477243 \
			115.923186955 139.616477243 115.923186955 79.5654629582 \
			115.923186955 79.5654629582 86.0904001986 82.5680136724 \
			52.5285150975 79.5654629582 -fill $SharedEnv(ButtonColor) -outline \
			black -smooth 1 -splinesteps 12 -stipple $SharedEnv(ButtonStipple2) \
			-tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create line 70.307360424 \
			52.5436151642 70.3402444446 43.5514838991 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create line 73.8697960001 \
			52.5436151642 73.9039953816 43.5514838991 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$SharedEnv(CurrentCanvas) create polygon \
			45.3703113917 148.62523802 45.3703113917 148.62523802 \
			84.5571027282 151.627788734 123.743894065 148.62523802 \
			123.743894065 148.62523802 120.181458489 154.630339448 \
			84.5571027282 157.632890163 48.9327469678 154.630339448 \
			45.3703113917 148.62523802 -fill white -outline black -smooth 1 \
			-splinesteps 12 -stipple $SharedEnv(ButtonStipple2) -tags {} -width \
			1]
	}}
	{Button Print {
		set i [$SharedEnv(CurrentCanvas) create polygon 4.0 2.0 29.0 \
			2.0 29.0 2.0 153.0 2.0 153.0 2.0 178.0 2.0 178.0 27.0 178.0 27.0 \
			178.0 141.0 178.0 141.0 178.0 166.0 153.0 166.0 153.0 166.0 29.0 \
			166.0 29.0 166.0 4.0 166.0 4.0 141.0 4.0 141.0 4.0 27.0 4.0 27.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} -tags \
			{} -width 1]
		set i [$SharedEnv(CurrentCanvas) create polygon 18.112852546 \
			78.1413823784 29.689679783 78.1413823784 29.689679783 \
			78.1413823784 145.509246403 78.1413823784 145.509246403 \
			78.1413823784 157.08607364 78.1413823784 157.08607364 \
			92.3229957436 157.08607364 92.3229957436 157.08607364 \
			106.520317973 157.08607364 106.520317973 157.08607364 \
			120.701931339 145.509246403 120.701931339 145.509246403 \
			120.701931339 29.689679783 120.701931339 29.689679783 \
			120.701931339 18.112852546 120.701931339 18.112852546 \
			106.520317973 18.112852546 106.520317973 18.112852546 \
			92.3229957436 18.112852546 92.3229957436 -fill \
			$SharedEnv(ButtonColor) -outline #000000000000 -smooth 0 \
			-splinesteps 12 -stipple $SharedEnv(ButtonStipple1) -tags {} -width \
			1]
		set i [$SharedEnv(CurrentCanvas) create polygon \
			29.6939543042 121.928678622 34.3263950074 121.928678622 \
			34.3263950074 121.928678622 140.872531179 121.928678622 \
			140.872531179 121.928678622 145.504971882 121.928678622 \
			145.504971882 130.395811007 145.504971882 130.395811007 \
			145.504971882 134.629377198 145.504971882 134.629377198 \
			145.504971882 143.096509583 140.872531179 143.096509583 \
			140.872531179 143.096509583 34.3263950074 143.096509583 \
			34.3263950074 143.096509583 29.6939543042 143.096509583 \
			29.6939543042 134.629377198 29.6939543042 134.629377198 \
			29.6939543042 130.395811007 29.6939543042 130.395811007 -fill \
			$SharedEnv(ButtonColor) -outline #000000000000 -smooth 0 \
			-splinesteps 12 -stipple $SharedEnv(ButtonStipple2) -tags {} -width \
			1]
		set i [$SharedEnv(CurrentCanvas) create polygon \
                        37.0084235148 76.6096100677 37.0084235148 76.6096100677 \
                        136.511731999 76.6096100677 136.511731999 76.6096100677 \
                        136.511731999 54.2857931245 148.949645559 31.9619761813 \
                        148.949645559 31.9619761813 49.4463370753 31.9619761813 \
                        49.4463370753 31.9619761813 37.0084235148 54.2857931245 \
			37.0084235148 76.6096100677 -fill \
			$SharedEnv(ButtonColor) -outline #000000000000 -smooth 0 \
			-splinesteps 12 -stipple $SharedEnv(ButtonStipple2) -tags {} -width \
			1]
		set i [$SharedEnv(CurrentCanvas) create polygon \
			148.978487615 31.8672316384 142.932094742 42.6870925685 \
			150.25141243 42.6870925685 -fill #FFFFFFFFFFFF -outline black \
			-smooth 0 -splinesteps 12 -stipple $SharedEnv(ButtonStipple1) -tags \
			{} -width 1]
	}}
}

#----------------------------------------------------------------------
# Toolbar along the left side of the nam editor window
#----------------------------------------------------------------------
Editor instproc build-toolbar {view toolbar mainWindow} {
  $self instvar magnification viewOffset SharedEnv balloon_ \
        state_buttons_ edit_state_ agent_types_

  set magnification 1.0
  set viewOffset(x) 0.0
  set viewOffset(y) 0.0

  set agent_types_ {CBR TCP TCP/FullTCP TCP/Reno TCP/NewReno \
                   TCP/Vegas TCP/Sack1 TCP/Fack TCPSink \
                   TCPSink/DelAck TCPSink/Sack1}

  # Build toolbars
  frame $toolbar.leftside
  button $toolbar.leftside.select -bitmap "select" \
                                  -command "$self popupCanvasSet select" \
                                  -highlightthickness 0 -borderwidth 1
  set state_buttons_(select) $toolbar.leftside.select

  button $toolbar.leftside.addnode -bitmap "addnode" \
                              -command "set SharedEnv(MenuSelect) 1; \
			                $self popupCanvasSet addnode" \
                              -highlightthickness 0 -borderwidth 1
  set state_buttons_(addnode) $toolbar.leftside.addnode

  button $toolbar.leftside.addlink -bitmap "addlink" \
                              -command "set SharedEnv(MenuSelect) 1; \
                                        $self popupCanvasSet addlink" \
                              -highlightthickness 0 -borderwidth 1
  set state_buttons_(addlink) $toolbar.leftside.addlink

  button $toolbar.leftside.addagent -bitmap "cut" \
                        -command "$self popupCanvasSet addagent" \
                        -highlightthickness 0 -borderwidth 1
  set state_buttons_(addagent) $toolbar.leftside.addagent

  # Create drop down list of agents
  eval tk_optionMenu $toolbar.leftside.agent_list agent_type $agent_types
  $toolbar.leftside.agent_list configure -width 16
  #bind $toolbar.leftside.agent_list.menu <Leave> [list $self popupCanvasSet {$agent_type}]
  bind $toolbar.leftside.agent_list.menu <Leave> {puts $agent_type}

  button $toolbar.leftside.add_traffic_source -bitmap "cut" \
                        -command "$self popupCanvasSet add_traffic_source" \
                        -highlightthickness 0 -borderwidth 1
  set state_buttons_(add_traffic_source) $toolbar.leftside.add_traffic_source
  
  button $toolbar.leftside.deleteobject -bitmap "cut" \
                        -command "$self popupCanvasSet deleteobject" \
                        -highlightthickness 0 -borderwidth 1
  set state_buttons_(deleteobject) $toolbar.leftside.deleteobject

  # Initial state: select
  $self popupCanvasSet select
	

  pack $toolbar.leftside.select \
       $toolbar.leftside.addnode \
       $toolbar.leftside.addlink \
       $toolbar.leftside.deleteobject \
       $toolbar.leftside.addagent \
       $toolbar.leftside.agent_list \
       $toolbar.leftside.add_traffic_source \
       -side left -ipady 5

	$balloon_ balloon_for $toolbar.leftside.select "Select"
	$balloon_ balloon_for $toolbar.leftside.addnode "Add Node"
	$balloon_ balloon_for $toolbar.leftside.addlink "Add Link"
	$balloon_ balloon_for $toolbar.leftside.deleteobject "Delete Object"

  frame $toolbar.rightside
  button $toolbar.rightside.zoomin -bitmap "zoomin" \
                                   -command "$view zoom 1.6" \
                                   -highlightthickness 0 \
                                   -borderwidth 1

  button $toolbar.rightside.zoomout -bitmap "zoomout" \
                                    -command "$view zoom 0.625" \
                                    -highlightthickness 0 \
                                    -borderwidth 1

  pack $toolbar.rightside.zoomin $toolbar.rightside.zoomout -side left -ipady 5

  $balloon_ balloon_for $toolbar.rightside.zoomin "ZoomIn"
  $balloon_ balloon_for $toolbar.rightside.zoomout "ZoomOut"

  pack $toolbar.leftside -side left 
  pack $toolbar.rightside -side right
}

Editor instproc createIcon { name width height value args} {
        $self instvar SharedEnv

        set SharedEnv(CurrentCanvas) $name

        bind $name <Enter> "
                        %W configure -bg $SharedEnv(ActiveBackground)
	if {[info exists SharedEnv(Msg_$value)]} {
                                set SharedEnv(Message) \$SharedEnv(Msg_$value)
	} else {
                                set SharedEnv(Message) $value
	}
                        $self enterToolButtons %W %x %y $value
        "
        bind $name <Leave> "
                        %W configure -bg $SharedEnv(Background)
                        set SharedEnv(Message) \[$self setMessage\]
                        $self leaveToolButtons %W
        "

        return $name
}

Editor instproc build-agentbar {view w mainW} {
        $self instvar magnification viewOffset state_buttons_
        set magnification 1.0
        set viewOffset(x) 0.0
        set viewOffset(y) 0.0
        frame $w.f
        pack $w.f -side top

        set agent_types {CBR TCP TCP/FullTCP TCP/Reno TCP/NewReno \
                         TCP/Vegas TCP/Sack1 TCP/Fack TCPSink \
                         TCPSink/DelAck TCPSink/Sack1}

#tk_optionMenu -command "puts $agent_type" $w.f.agent_list agent_type CBR TCP TCP/FullTCP
        eval tk_optionMenu $w.f.agent_list agent_type $agent_types
        $w.f.agent_list configure -width 16
        bind $w.f.agent_list <Leave> {puts "$agent_type"}

        button $w.f.b -text "CBR" \
                        -command "$self popupCanvasSet CBR" \
                        -highlightthickness 0 -borderwidth 1 
        set state_buttons_(CBR) $w.f.b

        button $w.f.b1 -text "TCP" \
                        -command "$self popupCanvasSet TCP" \
                        -highlightthickness 0 -borderwidth 1
        set state_buttons_(TCP) $w.f.b1
        button $w.f.b2 -text "TCP/FullTcp" \
                        -command "$self popupCanvasSet TCP/FullTcp" \
                        -highlightthickness 0 -borderwidth 1
        set state_buttons_(TCP/FullTcp) $w.f.b2
        button $w.f.b3 -text "TCP/Reno" \
                        -command "$self popupCanvasSet TCP/Reno" \
                        -highlightthickness 0 -borderwidth 1
        set state_buttons_(TCP/Reno) $w.f.b3
        button $w.f.b4 -text "TCP/Newreno" \
                        -command "$self popupCanvasSet TCP/Newreno" \
                        -highlightthickness 0 -borderwidth 1
        set state_buttons_(TCP/Newreno) $w.f.b4
        button $w.f.b5 -text "TCP/Vegas" \
                        -command "$self popupCanvasSet TCP/Vegas" \
                        -highlightthickness 0 -borderwidth 1
        set state_buttons_(TCP/Vegas) $w.f.b5
        button $w.f.b6 -text "TCP/Sack1" \
                        -command "$self popupCanvasSet TCP/Sack1" \
                        -highlightthickness 0 -borderwidth 1
        set state_buttons_(TCP/Sack1) $w.f.b6
        button $w.f.b7 -text "TCP/Fack" \
                        -command "$self popupCanvasSet TCP/Fack" \
                        -highlightthickness 0 -borderwidth 1
        set state_buttons_(TCP/Fack) $w.f.b7
        button $w.f.b8 -text "TCPSink" \
                        -command "$self popupCanvasSet TCPSink" \
                        -highlightthickness 0 -borderwidth 1
        set state_buttons_(TCPSink) $w.f.b8
        button $w.f.b9 -text "TCPSink/DelAck" \
                        -command "$self popupCanvasSet TCPSink/DelAck" \
                        -highlightthickness 0 -borderwidth 1
        set state_buttons_(TCPSink/DelAck) $w.f.b9
        button $w.f.b10 -text "TCPSink/Sack1" \
                        -command "$self popupCanvasSet TCPSink/Sack1" \
                        -highlightthickness 0 -borderwidth 1
        set state_buttons_(TCPSink/Sack1) $w.f.b10

        pack $w.f.agent_list -side top -ipady 3 -fill x

       # pack $w.f.agent_list $w.f.b $w.f.b1 $w.f.b2 $w.f.b3 $w.f.b4 $w.f.b5 \
#	     $w.f.b6 $w.f.b7 $w.f.b8 $w.f.b9 $w.f.b10 \
#  	     -side top -ipady 3 -fill x
}

  set SharedEnv(CurrentPageId) 1
  set SharedEnv(CurrentCanvas) "" 
  set SharedEnv(PageWidth) 8.5i
  set SharedEnv(PageHeight) 11i
  set SharedEnv(PageVisX) 3i
  set SharedEnv(PageVisY) 2i 
  set SharedEnv(SlideBG) white
  set SharedEnv(Landscape) 0 
  set SharedEnv(ColorWidth) 400      
  set SharedEnv(ColorHeight) 100
  set SharedEnv(PrivateCmap) ""
  set SharedEnv(ButtonOrient) left
  set SharedEnv(ButtonCount)  10
  set SharedEnv(ButtonSize) .35i
  set SharedEnv(ButtonRelief) flat
  set SharedEnv(ButtonStipple1) gray50
  set SharedEnv(ButtonStipple2) {}
  set SharedEnv(ButtonColor) cyan
  set SharedEnv(Fill) "" 
  set SharedEnv(MenuSelect) 0
  button .b
  set SharedEnv(Background) [.b cget -bg]
  set SharedEnv(ActiveBackground) [.b cget -activebackground]
  destroy .b
  set SharedEnv(DefButtonColor) [. cget -bg]
  set SharedEnv(Cut) ""
  set SharedEnv(Copy) ""
  set SharedEnv(Message) "Select"
  set SharedEnv(Xpos) 0
  set SharedEnv(Ypos) 0
  set SharedEnv(Smooth) 0
  set SharedEnv(Outline) black
  set SharedEnv(Stipple) ""
  set SharedEnv(Arrow) none
  set SharedEnv(JoinStyle) miter 
  set SharedEnv(Grid) 0
  set SharedEnv(GridX) .25i
  set SharedEnv(GridY) .25i
  set SharedEnv(UnitX) i
  set SharedEnv(UnitY) i
  set SharedEnv(ScreenH) [winfo screenheight .]
  set SharedEnv(ScreenW) [winfo screenwidth .]
  set SharedEnv(Gravity) 1
  set SharedEnv(GravityVal) 30
  set SharedEnv(Sub-Cursor) ""
  
  set SharedEnv(Start) ""
  set SharedEnv(top_left_corner) bottom_right_corner
  set SharedEnv(top_side) bottom_side
  set SharedEnv(top_right_corner) bottom_left_corner
  set SharedEnv(right_side) left_side
  set SharedEnv(bottom_right_corner) top_left_corner
  set SharedEnv(bottom_side) top_side
  
  set SharedEnv(bottom_left_corner) top_right_corner
  set SharedEnv(left_side) right_side
  set SharedEnv(FixedAspect) True
  
  set menulist_ {
  menu Menu {} {}
  {
     {
    	file File {-underline 0} {}
      	{
           {
		         new New {-underline 0} {$self newFile}
	       }
               { 
		    open Open {-underline 0} {$self openFile}
                }
		{
		    open Save {-underline 0} {$self saveFile}
		}
		{
		    saveas "Save As..." {-underline 4} { $self saveAsFile}
		}
		{
		    sep1 {} {} {}
		}
		{
				run "Run Ns" {-underline 1} { $self saveFile; $self runNs }
		}
		{
		    sep1 {} {} {}
		}
		{
		    exit Close {-underline 1} "destroy $editorwindow_"
		}
	}
   }
	{
		edit Edit {-underline 0} {}
		{
			{
				select Select {-underline 0} {$self popupCanvasSet Select}
			}
			{
				oval Node {-underline 0} {set SharedEnv(MenuSelect) 1; $self popupCanvasSet Node}
			}
			{
				rect Link {-underline 0} {set SharedEnv(MenuSelect) 1; $self popupCanvasSet Link}
			}
                        {
			    cut Cut {-underline 0} {$self popupCanvasSet Cut}
                        }
#			{
#				line Queue {-underline 0} \
#	                        {set SharedEnv(MenuSelect) 1; $self popupCanvasSet Queue}
#			}
#			{
#				line Track {-underline 0} \
#	                        {set SharedEnv(MenuSelect) 1; $self popupCanvasSet Track}
#			}
#			{
#				paste Paste {-underline 0} \
#	                        {$self popupCanvasSet Paste}
#			}
		}
	}

        {
	    agent Agent {-underline 0} {}
                {
                        {
			    tcp TCP {-underline 0} { $self popupCanvasSet TCP }
                        }
                        {
                            tcp-full TCP/FullTcp {-underline 0} { $self popupCanvasSet TCP/FullTcp }
                        }
                        {
                            tcp-reno TCP/Reno {-underline 0} { $self popupCanvasSet TCP/Reno }
                        }
                        {
                            tcp-newreno TCP/Newreno {-underline 0} { $self popupCanvasSet TCP/Newreno }
                        }
                        {
                            tcp-vegas TCP/Vegas {-underline 0} {  $self popupCanvasSet TCP/Vegas }
                        }
                        {
                            tcp-sack1 TCP/Sack1 {-underline 0} {  $self popupCanvasSet TCP/Sack1 }
                        } 
                        {
                            tcp-fack TCP/Fack {-underline 0} { $self popupCanvasSet TCP/Fack }
                        }
                        {
			    tcpsink TCPSink {-underline 0} {  $self popupCanvasSet TCPSink }
                        }
                        {
                            tcpsink-delack TCPSink/DelAck {-underline 0} { $self popupCanvasSet TCPSink/DelAck }
                        }
                        {
                            tcpsink-sack1 TCPSink/Sack1 {-underline 0} { $self popupCanvasSet TCPSink/Sack1 }
                        }

		}

	 }
	{
		preferences View {-underline 0} {} 
		{
#		        {
#			        agent AgentBar {-underline 0} {}
#			}
			{
				buttons ToolBar {-underline 0}
				{$self buttonReq .buttons .radiosr}
			}

			{
				buttons "Color Palette" {-underline 0}
				{$self colorPaletteReq .colorpalette \
                                {0000 3300 6600 9900 CC00 FF00} \
                                {0000 3300 6600 9900 CC00 FF00} \
                                {0000 3300 6600 9900 CC00 FF00} \
                                .colorsp}
			}
		}
	}
	{
		quick_help Quick_Help {-underline 0} {}
		{
                        {
			    help HowToUse {-underline 0}
			    {$self helpReq}
                        }
		}
	}
}
}
}
