import 'package:flutter/material.dart';
import 'package:syncfusion_flutter_charts/charts.dart';
import 'package:syncfusion_flutter_core/core.dart';

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
  late RangeController rangeController;

  @override
  void initState() {
    super.initState();

    rangeController = RangeController(
      start: DateTime(2025, 2, 17),
      end: DateTime(2025, 2, 18),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: AppBar(
          title: const Text('Syncfusion Flutter chart'),
        ),
        body: Column(children: [
        SfCartesianChart(
          primaryYAxis: NumericAxis(),
          primaryXAxis: DateTimeAxis(
            minimum: DateTime(2000),
            maximum: DateTime(
              2031,
            ),
            initialVisibleMinimum: this.rangeController.start,
            initialVisibleMaximum: this.rangeController.end,
            rangeController: this.rangeController,
          ),
        ),
        IconButton(
            icon: Icon(Icons.arrow_upward),
            onPressed: () {
              this.rangeController.start = DateTime(2025, 2, 17);
              this.rangeController.end = DateTime(2025, 2, 24);
              setState(() {});
            }),
      ]));
  }
}
