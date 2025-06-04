import 'package:flutter/material.dart';
import 'package:syncfusion_flutter_charts/charts.dart';

void main() {
  return runApp(_ChartApp());
}

class _ChartApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      theme: ThemeData(primarySwatch: Colors.blue, useMaterial3: false),
      home: _MyHomePage(),
    );
  }
}

class _MyHomePage extends StatefulWidget {
  // ignore: prefer_const_constructors_in_immutables
  _MyHomePage({Key? key}) : super(key: key);

  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<_MyHomePage> {
  Map<DateTimeIntervalType, List<_SalesData>> data = {
    DateTimeIntervalType.months: [
      _SalesData(DateTime(2000, 1), 35),
      _SalesData(DateTime(2000, 2), 28),
      _SalesData(DateTime(2000, 3), 34),
      _SalesData(DateTime(2000, 4), 32),
      _SalesData(DateTime(2000, 5), 40)
    ],
    DateTimeIntervalType.years: [
      _SalesData(DateTime(2000), 35),
      _SalesData(DateTime(2001), 28),
      _SalesData(DateTime(2002), 34),
      _SalesData(DateTime(2003), 32),
      _SalesData(DateTime(2004), 40)
    ],
  };

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Syncfusion Flutter chart'),
      ),
      body: Row(
        children: [
          for (DateTimeIntervalType intervalType in [DateTimeIntervalType.months, DateTimeIntervalType.years])
            Expanded(
              child: Column(
                children: [
                  for (bool labelsAtBeginning in [true, false])
                    //Initialize the chart widget
                    Expanded(
                      child: SfCartesianChart(
                        primaryXAxis: DateTimeAxis(
                          intervalType: intervalType,
                          initialVisibleMaximum: data[intervalType]![2].year,
                          labelsAtBeginning: labelsAtBeginning,
                        ),
                        // Chart title
                        title: ChartTitle(
                            text: labelsAtBeginning
                                ? 'With labels at beginning'
                                : 'Labels placed dynamically'),
                        zoomPanBehavior: ZoomPanBehavior(enablePanning: true),
                        // Enable tooltip
                        tooltipBehavior: TooltipBehavior(enable: true),
                        series: <CartesianSeries<_SalesData, DateTime>>[
                          LineSeries<_SalesData, DateTime>(
                              dataSource: data[intervalType],
                              xValueMapper: (_SalesData sales, _) => sales.year,
                              yValueMapper: (_SalesData sales, _) =>
                                  sales.sales,
                              name: 'Sales',
                              // Enable data label
                              dataLabelSettings:
                                  DataLabelSettings(isVisible: true))
                        ],
                      ),
                    ),
                ],
              ),
            ),
        ],
      ),
    );
  }
}

class _SalesData {
  _SalesData(this.year, this.sales);

  final DateTime year;
  final double sales;
}
