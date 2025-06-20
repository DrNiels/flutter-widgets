import 'package:flutter/material.dart';
import 'package:syncfusion_flutter_charts/charts.dart';
import 'package:syncfusion_flutter_charts/sparkcharts.dart';

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
  List<_SalesData> data = [
    _SalesData('Jan', 35),
    _SalesData('Feb', 28),
    _SalesData('Mar', 34),
    _SalesData('Apr', 32),
    _SalesData('May', 40)
  ];
  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: AppBar(
          title: const Text('Syncfusion Flutter chart'),
        ),
        body: Wrap(children: [
          //Initialize the chart widget
          SizedBox(
            width: 400,
            child: SfCartesianChart(
              title: ChartTitle(text: 'Regular Hide'),
              primaryYAxis: NumericAxis(),
              primaryXAxis: DateTimeAxis(
                intervalType: DateTimeIntervalType.hours,
                interval: 1,
                labelIntersectAction: AxisLabelIntersectAction.hide,
                minimum: DateTime(2025, 2, 17),
                maximum: DateTime(2025, 2, 18),
              ),
            ),
          ),
          SizedBox(
            width: 400,
            child: SfCartesianChart(
              title: ChartTitle(text: 'Uniform Hide'),
              primaryYAxis: NumericAxis(),
              primaryXAxis: DateTimeAxis(
                intervalType: DateTimeIntervalType.hours,
                interval: 1,
                labelIntersectAction: AxisLabelIntersectAction.hideUniform,
                minimum: DateTime(2025, 2, 17),
                maximum: DateTime(2025, 2, 18),
              ),
            ),
          ),
        ]));
  }
}

class _SalesData {
  _SalesData(this.year, this.sales);

  final String year;
  final double sales;
}
